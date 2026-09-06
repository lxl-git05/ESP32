"""Convert sibling HTML files to ESP32-ready C/C++ string fragments."""

from __future__ import annotations

import os
import re
import tempfile
from pathlib import Path


HTML_COMMENT_RE = re.compile(r"<!--.*?-->", re.DOTALL)
STYLE_BLOCK_RE = re.compile(
    r"(<style\b[^>]*>)(.*?)(</style\s*>)",
    re.IGNORECASE | re.DOTALL,
)
CSS_COMMENT_RE = re.compile(r"/\*.*?\*/", re.DOTALL)


def remove_comments(html: str) -> str:
    """Remove HTML comments and CSS comments inside <style> elements."""
    html = HTML_COMMENT_RE.sub("", html)

    def clean_style(match: re.Match[str]) -> str:
        opening_tag, css, closing_tag = match.groups()
        return opening_tag + CSS_COMMENT_RE.sub("", css) + closing_tag

    return STYLE_BLOCK_RE.sub(clean_style, html)


def escape_cpp_string(text: str) -> str:
    """Escape one text line for use inside a C/C++ string literal."""
    return text.replace("\\", "\\\\").replace('"', '\\"').replace("\t", "\\t")


def convert_html(html: str) -> str:
    """Return ESP32-ready adjacent string literal lines."""
    output_lines: list[str] = []

    for line in remove_comments(html).splitlines():
        if not line.strip():
            continue
        output_lines.append(f'    "{escape_cpp_string(line)}\\r\\n"')

    if not output_lines:
        return ""
    return "\n".join(output_lines) + "\n"


def write_utf8_atomically(path: Path, content: str) -> None:
    """Replace a text file only after its complete new content is written."""
    temporary_path: Path | None = None
    try:
        with tempfile.NamedTemporaryFile(
            mode="w",
            encoding="utf-8",
            newline="\n",
            dir=path.parent,
            prefix=f".{path.name}.",
            suffix=".tmp",
            delete=False,
        ) as temporary_file:
            temporary_file.write(content)
            temporary_path = Path(temporary_file.name)

        os.replace(temporary_path, path)
        temporary_path = None
    finally:
        if temporary_path is not None:
            temporary_path.unlink(missing_ok=True)


def convert_file(html_path: Path) -> bool:
    """Convert one HTML file and return whether an existing TXT was replaced."""
    html = html_path.read_text(encoding="utf-8-sig")
    output_path = html_path.with_suffix(".txt")
    was_overwritten = output_path.exists()
    write_utf8_atomically(output_path, convert_html(html))
    return was_overwritten


def find_html_files(directory: Path) -> list[Path]:
    """Find non-recursive .html files, including uppercase suffix variants."""
    return sorted(
        (
            path
            for path in directory.iterdir()
            if path.is_file() and path.suffix.lower() == ".html"
        ),
        key=lambda path: path.name.casefold(),
    )


def process_directory(directory: Path) -> int:
    """Convert all HTML files directly inside a directory."""
    html_files = find_html_files(directory)
    if not html_files:
        print(f"未找到 HTML 文件：{directory}")
        return 0

    succeeded = 0
    overwritten = 0
    failed = 0

    for html_path in html_files:
        output_path = html_path.with_suffix(".txt")
        try:
            was_overwritten = convert_file(html_path)
        except (OSError, UnicodeError) as error:
            failed += 1
            print(f"[失败] {html_path.name}：{error}")
            continue

        succeeded += 1
        if was_overwritten:
            overwritten += 1
            print(f"[覆盖] {output_path.name}")
        else:
            print(f"[生成] {output_path.name}")

    print(
        f"处理完成：总计 {len(html_files)}，成功 {succeeded}，"
        f"其中覆盖 {overwritten}，失败 {failed}。"
    )
    return 1 if failed else 0


def main() -> int:
    script_directory = Path(__file__).resolve().parent
    return process_directory(script_directory)


if __name__ == "__main__":
    raise SystemExit(main())

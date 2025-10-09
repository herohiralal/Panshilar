from pathlib import Path
from typing import Optional

def run(
        input_path: str | Path,
        output_path: Optional[str | Path] = None,
        bytes_per_line: int = 16
    ) -> None:

    p = Path(input_path)
    data = p.read_bytes()

    lines: list[str] = []
    hex_bytes = [f"0x{b:02X}" for b in data]
    for i in range(0, len(hex_bytes), bytes_per_line):
        lines.append(", ".join(hex_bytes[i:i+bytes_per_line]))

    array_decl = ",\n        ".join(lines) if lines else ""

    c: list[str] = []
    c.append(f"// generated from {p.name}, do not edit")
    c.append("#ifndef INLINED_FILE_INCLUSION_NAME")
    c.append("    #error \"You must define INLINED_FILE_INCLUSION_NAME to be the name of the inlined file (without extension)\"")
    c.append("#else")
    c.append("")
    c.append("    #define AUTOGEN_XXX_MY_MACRO_COMBINE(x, y) x##y")
    c.append("")
    c.append("    static const unsigned char XXX_MY_MACRO_COMBINE(INLINED_FILE_INCLUSION_NAME, Contents) [] = {")
    if array_decl:
        c.append(f"        {array_decl}")
    c.append("    };")
    c.append("")
    c.append("    static const unsigned long long XXX_MY_MACRO_COMBINE(INLINED_FILE_INCLUSION_NAME, Size) = sizeof( XXX_MY_MACRO_COMBINE(INLINED_FILE_INCLUSION_NAME, Contents) );")
    c.append("")
    c.append("    #undef AUTOGEN_XXX_MY_MACRO_COMBINE")
    c.append("    #undef INLINED_FILE_INCLUSION_NAME")
    c.append("")
    c.append("#endif")
    c.append("")

    out = "\n".join(c)

    if output_path:
        Path(output_path).write_text(out, encoding="utf-8")
    else:
        print(out)

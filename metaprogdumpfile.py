from pathlib import Path
from typing import Optional

def run(
        inputPath: str | Path,
        outputPath: Optional[str | Path] = None,
        bytesPerLine: int = 16
    ) -> None:

    p = Path(inputPath)
    data = p.read_bytes()

    lines: list[str] = []
    hexBytes = [f"0x{b:02X}" for b in data]
    for i in range(0, len(hexBytes), bytesPerLine):
        lines.append(", ".join(hexBytes[i:i+bytesPerLine]))

    arrDecl = ",\n        ".join(lines) if lines else ""

    c: list[str] = []
    c.append(f"// generated from {p.name}, do not edit")
    c.append("#ifndef INLINED_FILE_INCLUSION_NAME")
    c.append("    #error \"You must define INLINED_FILE_INCLUSION_NAME to be the name of the inlined file (without extension)\"")
    c.append("#else")
    c.append("")
    c.append("    #define AUTOGEN_XXX_MY_MACRO_COMBINE_1(x, y) x##y")
    c.append("    #define AUTOGEN_XXX_MY_MACRO_COMBINE(x, y) AUTOGEN_XXX_MY_MACRO_COMBINE_1(x, y)")
    c.append("")
    c.append("    static const unsigned char AUTOGEN_XXX_MY_MACRO_COMBINE(INLINED_FILE_INCLUSION_NAME, Contents) [] = {")
    if arrDecl:
        c.append(f"        {arrDecl}")
    c.append("    };")
    c.append("")
    c.append("    static const unsigned long long AUTOGEN_XXX_MY_MACRO_COMBINE(INLINED_FILE_INCLUSION_NAME, Size) = sizeof( AUTOGEN_XXX_MY_MACRO_COMBINE(INLINED_FILE_INCLUSION_NAME, Contents) );")
    c.append("")
    c.append("    #undef AUTOGEN_XXX_MY_MACRO_COMBINE")
    c.append("    #undef AUTOGEN_XXX_MY_MACRO_COMBINE_1")
    c.append("    #undef INLINED_FILE_INCLUSION_NAME")
    c.append("")
    c.append("#endif")
    c.append("")

    out = "\n".join(c)

    if outputPath:
        Path(outputPath).write_text(out, encoding="utf-8")
    else:
        print(out)

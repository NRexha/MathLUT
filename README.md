# MathLUT

MathLUT is a lightweight desktop application designed to make the creation of lookup tables (LUTs) more intuitive. You can write GLSL-like functions directly in the built-in code editor, inspect their behavior through curve graphs, preview the resulting texture, and export it as a PNG. 

>**Note:** The tool was developed as a three-day challenge. It was my first time building a tool from scratch without relying on an application framework. The project was a great opportunity to explore C++, OpenGL compute shaders.

<img width="1284" height="767" alt="MathLUT" src="https://github.com/user-attachments/assets/6caa176d-369e-4b0a-a471-bfd9956ae954" />

## Download

1. Download the latest Windows build from the [Releases](https://github.com/NRexha/MathLUT/releases) page.
2. Extract the ZIP archive and launch `MathLUT.exe`.

> **Note:** Keep the `src` folder alongside the executable. MathLUT loads its shaders and function files at runtime.

## Writing functions

Write your function directly in the built-in code editor. MathLUT uses a compact, GLSL-like syntax to define the output channels of a LUT.

For example:

```glsl
r = x;
g = x * x;
b = 1.0 - x;
a = 1.0;
```

Each assignment defines an output channel. In this example, red follows the input, green follows its square, blue is inverted, and alpha remains fully opaque.

You can combine built-in variables, mathematical constants, and supported GLSL functions to create more complex transformations. There's also autocomplete!
Compilation feedback is displayed in the console, helping you identify errors while editing.

## Built-in variables

| Name | Description |
|---|---|
| `x` | X-coordinate input |
| `PI` | 3.14159265358979323846 |
| `TAU` | 6.28318530717958647692 |
| `E` | 2.71828182845904523536 |
| `_Time` | Elapsed time |

## Built with

- C++
- OpenGL 4.3
- [GLAD](https://github.com/dav1dde/glad)
- [Dear ImGui — Docking](https://github.com/ocornut/imgui/tree/docking)
- [ImGuiColorTextEdit](https://github.com/goossens/ImGuiColorTextEdit)
- [GLFW](https://www.glfw.org/)

## License

See the [LICENSE](LICENSE) file for licensing information.

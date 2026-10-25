# MathLUT

MathLUT is a lightweight desktop application designed to make the creation of lookup tables (LUTs) more intuitive. You can write GLSL-like functions directly in the built-in code editor, inspect their behavior through curve graphs, preview the resulting texture, and export it as a PNG. 

>**Note:** The tool was developed as a three-day challenge. It was my first time building a tool from scratch without relying on an application framework. The project was a great opportunity to explore C++ and OpenGL compute shaders.

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

You can also assign multiple channels at once using vector types:

```glsl
rgb = vec3(x, x * x, 1.0 - x);
a = 1.0;
```
Each assignment defines an output channel. In this example, red follows the input, green follows its square, blue is inverted, and alpha remains fully opaque.

You can combine built-in variables, mathematical constants, and supported GLSL functions to create more complex transformations. There's also autocomplete!
Compilation feedback is displayed in the console, helping you identify errors while editing.

<img width="1284" height="767" alt="MathLUT_Func" src="https://github.com/user-attachments/assets/45650c11-ced7-4e70-916e-bf3cc9d537cc" />


## Built-in variables

| Name | Description |
|---|---|
| `x` | X-coordinate input |
| `PI` | 3.14159265358979323846 |
| `TAU` | 6.28318530717958647692 |
| `E` | 2.71828182845904523536 |
| `_Time` | Elapsed time |

## Example use case
In this Unreal Engine material, numerous trigonometric operations (`sin`, `cos`, `atan2`, etc.) were used throughout the graph, although only a `sin` and `cos` are shown in this screenshot for practice purpose. Their cumulative cost risked creating a GPU bottleneck. By baking some of these calculations into a small 32 × 4 px LUT texture, we could replace runtime math with texture samples, shifting part of the workload to texture lookups and potentially reducing pressure on shader execution resources.

<img width="1687" height="744" alt="Usage" src="https://github.com/user-attachments/assets/ce1f82be-477a-404f-aa66-4ccd6fb70187" />

## Dependencies

- C++ 20
- OpenGL 4.3
- [GLAD](https://github.com/dav1dde/glad)
- [Dear ImGui — Docking](https://github.com/ocornut/imgui/tree/docking)
- [ImGuiColorTextEdit](https://github.com/goossens/ImGuiColorTextEdit)
- [GLFW](https://www.glfw.org/)
- [stb_image_write](https://github.com/nothings/stb)
- [JetBrains Mono Regular](https://www.jetbrains.com/lp/mono/)

## License

Released under <a href="/LICENSE">MIT</a> by <a href="https://github.com/NRexha">@NRexha</a>.

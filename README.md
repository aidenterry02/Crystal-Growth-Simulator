# Crystal Growth Simulation

This project simulates crystal growth using OpenGL and GLSL compute shaders. It demonstrates particle motion, rendering, and real-time updates using modern graphics programming techniques.

## Features
- Real-time particle simulation
- OpenGL-based rendering
- Compute shaders for parallelized computations
- Configurable simulation speed

---

## Requirements

### Libraries
Make sure the following libraries are installed on your system:
- **GLEW (2.1.0)**: [Download GLEW](http://glew.sourceforge.net/)
- **GLFW (3.4)**: [Download GLFW](https://www.glfw.org/)
- **GLM (1.0.1)**: [Download GLM](https://glm.g-truc.net/)

### Compiler and Tools
- **G++** (MinGW recommended for Windows)
- OpenGL-compatible graphics card

### Environment Setup
1. Install the above libraries.
2. Add G++ and necessary library paths to your system's PATH environment variable.

---

## How to Build and Run

### Using Predefined Tasks
This project includes JSON configuration files for building and running the simulation in Visual Studio Code.

### Steps:
1. Clone this repository:
   ```bash
   git clone https://github.com/YourGitHubUsername/CrystalGrowthSimulation.git
   cd CrystalGrowthSimulation
   ```

2. Open the project in **Visual Studio Code**.

3. Build the project using the predefined tasks:
   - Press `Ctrl+Shift+B` to open the build tasks menu.
   - Select `Build Crystal Simulation`.

4. Run the generated executable `CrystalSimulation` in your terminal or debugger.

---

## Configuration Details

### `c_cpp_properties.json`
This file configures IntelliSense for proper C++ autocompletion and linting.

Key paths:
- **Include Directories**:
  - `${workspaceFolder}/**`
  - `C:/Libraries/GLEW/glew-2.1.0/include`
  - `C:/Libraries/GLFW/glfw-3.4.bin.WIN64/include`
  - `C:/Libraries/GLM/glm-1.0.1`
- **Compiler Path**:
  - `C:/mingw64/bin/g++.exe`

### `tasks.json`
This file contains predefined build tasks:
1. **Build Crystal Simulation**:
   - Command: `g++`
   - Includes required libraries and paths.

2. **C/C++: g++.exe build active file**:
   - General-purpose build task for single files.

---

## Troubleshooting
- **Compilation Errors**:
  Ensure all required libraries are installed and properly referenced in `tasks.json` and `c_cpp_properties.json`.
- **Runtime Errors**:
  Verify your graphics drivers support OpenGL.
- **Debugging**:
  Enable OpenGL debugging with `glEnable(GL_DEBUG_OUTPUT);` for verbose output.

---

## Contributions
Feel free to open issues or submit pull requests to improve the project. Suggestions and feedback are welcome!

---

## License
This project is licensed under the [MIT License](LICENSE).


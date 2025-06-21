# neat - Image Presentation Tool

Welcome to **neat**, a C++ port of the original Python-based `neat-present` image presentation tool.

**Linux only**, at least for now.

[Download the latest flatpak release.](https://github.com/acrilique/neat-cpp/releases/download/v0.1.0/neat.flatpak)

## Purpose

Take an image and make a presentation out of it. Useful for diagrams, flowcharts, etc. The output file format of the app (.neatp) is a JSON file with the image base64 encoded and the coordinates of the points for the presentation.

## Building the Application

There are two main ways to build the project: a standard CMake build for local development and a Flatpak build for distribution.

### 1. Standard CMake Build (for development and testing)

This method is ideal for development and local testing.

1.  **Create a build directory:**
    ```bash
    mkdir build && cd build
    ```
2.  **Configure the project with CMake:**
    ```bash
    cmake ..
    ```
3.  **Compile the project:**
    ```bash
    make
    ```
    This will create an executable file named `neat` inside the `build` directory.

### 2. Flatpak Build (for distribution)

This method packages the application as a Flatpak for easy distribution and installation on different Linux distributions.

1.  **Install `flatpak-builder`** if you haven't already.
2.  **Install the kde Sdk runtime**:
    ```bash
    flatpak install flathub org.kde.Sdk//6.8
    ```
3.  **Build the Flatpak without installing it directly:**
    ```bash
    flatpak-builder --repo=repo --force-clean build-dir com.acrilique.neat.yml
    ```
4.  **Create a bundle from the repository:**
    ```bash
    flatpak build-bundle repo neat.flatpak com.acrilique.neat
    ```
    This will create a `neat.flatpak` file that you can share with others.

### Troubleshooting
You need Qt6 development packages installed to build this project. If you encounter issues, ensure that you have the necessary Qt6 development libraries installed on your system.

## Installing the Application

### From a Flatpak Bundle

If you have a `neat.flatpak` file, you can install it with the following command:

```bash
flatpak install --user neat.flatpak
```

### Running the Application

Once installed, there will be a Start Menu shortcut for the application with the name "Neat". You can also run the application from the command line with:

```bash
flatpak run com.acrilique.neat
```

## License

This project is licensed under the MIT License.

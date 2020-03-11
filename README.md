# Apoapse Pro Client
The Apoapse Pro Client is part of the Apoapse Pro messaging platform, offering a secure and decentralized collaborative space for communicating and sharing files.

![Apoapse Pro Login](https://apoapse.space/wp-content/uploads/2019/10/2_min.jpg)
![Apoapse Pro User Interface](https://apoapse.space/wp-content/uploads/2019/10/3_min.png)

## How to build
To compile the Apoapse Pro Client, you first need to compile the Apoapse Protocol as a static library that must be linked into the *ApoapseClientShared* dynamic library. As such, you first need to obtain and compile the Apoapse Protocol following the instructions on the Apoapse Protocol repository. With most of the third party libraries shared between these two repositories, both repositories must be placed into a same folder, alongside each other.

The client also use specific third party libraries that must be acquired manually and placed on the ThirdParty folder of the client repository. Here is their name and save location:
* [Chromium Embedded Framework](https://bitbucket.org/chromiumembedded/cef) - ThirdParty/cef_binary
* [OpenCV](https://opencv.org/) - ThirdParty/opencv

You can then compile the client using CMake or the Visual Studio solution provided.

## User Interface
The UI of the Apoapse Pro Client is made using web technologies relying on the Chromium Embedded Framework for rendering. The Apoapse Pro Client HTML UI must be placed in a folder called *ClientResources* alongside the client executable. See the [Apoapse Pro Client HTML UI repository](https://github.com/apoapse/ApoapseProClientHtmlUI) for more details.
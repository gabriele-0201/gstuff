<div id="top"></div>

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]


<!-- PROJECT LOGO -->
<br />
<div align="center">

  <h3 align="center">Welcome to gstuff!</h3>

  <p align="center">
    <a href="https://github.com/gabriele-0201/gstuff/"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/gabriele-0201/gstuff/">View Demo</a>
    ·
    <a href="https://github.com/gabriele-0201/gstuff/issues">Report Bug</a>
    ·
    <a href="https://github.com/gabriele-0201/gstuff/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

<!-- [![Product Name Screen Shot][product-screenshot]] -->

Gstuff is a very lightweight and deamon-less notification program built with X11 libraries that can show a box with customizable text in it.




### Built With

* [X11](https://github.com/freedesktop/xorg-libX11)



### Prerequisites

In order to use gstuff you should be running the **X11 window system**.
To compile gstuff from source you should have **cmake** and **make** installed.



### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/gabriele-0201/gstuff.git
   ```
2. Open it in a terminal
3. Create a buld/ directory
    ```sh
   mkdir ./build 
   ```
4. Run cmake to generate Makefile and config files
   ```sh
   cmake -S . -B ./build
   ```
5. Compile with make
   ```sh
   make -C ./build
   ```

Now your executable program sohould be at ./build/gstuff and you can run a test by executing
```sh
./build/gstuff "hello world"
```

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

Some use cases for it may be:
- Getting status of your system with a keybinding
- Having a customizable slider for display backlight and/or volume
- Integrating notification capabilities in your own project
- Showing reminders or timers

<!-- TODO: add some examples in an example directory and add images here -->



<!-- ROADMAP -->
## TODO/Roadmap

- [ ] Refactor code
- [ ] Add images in README
- [ ] Add some examples
- [ ] Add svg support

See the [open issues](https://github.com/gabriele-0201/gstuff/issues) for a full list of proposed features (and known issues).



<!-- CONTRIBUTING -->
## Contributing

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/othneildrew/Best-README-Template.svg?style=for-the-badge
[contributors-url]: https://github.com/othneildrew/Best-README-Template/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/othneildrew/Best-README-Template.svg?style=for-the-badge
[forks-url]: https://github.com/othneildrew/Best-README-Template/network/members
[stars-shield]: https://img.shields.io/github/stars/othneildrew/Best-README-Template.svg?style=for-the-badge
[stars-url]: https://github.com/othneildrew/Best-README-Template/stargazers
[issues-shield]: https://img.shields.io/github/issues/othneildrew/Best-README-Template.svg?style=for-the-badge
[issues-url]: https://github.com/othneildrew/Best-README-Template/issues
[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=for-the-badge
[license-url]: https://github.com/othneildrew/Best-README-Template/blob/master/LICENSE.txt
[contributors-url]: https://github.com/othneildrew/Best-README-Template/graphs/contributors
[forks-url]: https://github.com/gabriele-0201/gstuff/network/members
[stars-url]: https://github.com/gabriele-0201/gstuff/stargazers
[issues-url]: https://github.com/gabriele-0201/gstuff/issues
## Getting Started

In this section, we will hold hands as we stroll through the intial setup for CheesecakeOS following these steps:

1. [Launch an AWS EC2 instance](#create-an-aw2-ec2-instance)
2. [Fork/clone the CheesecakeOS repo](#forkclone-the-cheesecakeos-repo)
3. [Setup docker](#setup-docker)
4. [Image SD card with Raspberry Pi OS](#image-sd-card-with-raspberry-pi-os)
5. [Install USB to UART Drivers](#install-usb-to-uart-drivers)
6. [Connect Raspberry Pi Mini UART to computer through USB](#connect-raspberry-pi-mini-uart-to-computer-through-usb).
7. [Login to your Raspberry Pi through the console](#login-to-your-raspberry-pi-through-the-console)

#### Launch an AWS EC2 instance

This step is not requiried, especially if you have Debian Linux already. Otherwise, this can help sort out the environment. Head over to the [AWS EC2 Get Started Tutorial](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/EC2_GetStarted.html). If you decide to launch an instance, I recommend you choose from the free tier eligible 64-bit x86 Ubuntu options. Afterwards connect to your instance and continue to step [2](#forkclone-the-cheesecakeos-repo).

#### Fork/Clone the CheesecakeOS Repo

Since you are reading this, you already know where the [CheesecakeOS] repo is located, so go ahead and clone it (or your fork of it):

```bash
ccos4rbpi:~$ git clone git@github.com:jmielkeway/ccos4rbpi_volume1.git
Cloning into 'ccos4rbpi_volume1'...
remote: Enumerating objects: 109, done.
remote: Counting objects: 100% (109/109), done.
remote: Compressing objects: 100% (76/76), done.
remote: Total 109 (delta 37), reused 74 (delta 18), pack-reused 0
Receiving objects: 100% (109/109), 39.18 KiB | 7.84 MiB/s, done.
Resolving deltas: 100% (37/37), done.
```

#### Setup Docker

If you are interested in using the `build.sh` scripts included in each code directory, which use a docker container to manage the build environment, run the `setup-docker-aws.sh` script from the top level of the repository. This *_will_* install docker! It will also setup the ubuntu user as a member of the docker group. Root privleges are required.

```bash
ccos4rbpi:~$ ./setup-docker-aws.sh
```
\*\*\*
\*\*\*
\*\*\*
```bash
Hello from Docker!
This message shows that your installation appears to be working correctly.

To generate this message, Docker took the following steps:
 1. The Docker client contacted the Docker daemon.
 2. The Docker daemon pulled the "hello-world" image from the Docker Hub.
    (amd64)
 3. The Docker daemon created a new container from that image which runs the
    executable that produces the output you are currently reading.
 4. The Docker daemon streamed that output to the Docker client, which sent it
    to your terminal.

To try something more ambitious, you can run an Ubuntu container with:
 $ docker run -it ubuntu bash

Share images, automate workflows, and more with a free Docker ID:
 https://hub.docker.com/

For more examples and ideas, visit:
 https://docs.docker.com/get-started/
```

After the script completes and prints out the docker hello-world message, log out, and then back in for the docker group setting to take effect. Finally, build the ccos4rbpi image:

```bash
ccos4rbpi:~$ docker build -t ccos4rbpi -f Dockerfile .
```

After the image is built, the `build.sh` scripts can be used to build the sources.

#### Image SD Card with Raspberry Pi OS

Insert microSD card into computer. Open the Raspberry Pi Imager software. Select the RASPBERRY PI OS (32-BIT) option for the Operating System. Select the microSD card as the SD card. Click the WRITE button. This will write the OS to the card. At the same time, it will create the boot partition where we will ultimately place our own CheesecakeOS.

#### Install USB to UART Drivers

[Download and install the USB to UART drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers). This assumes your serial cable has a CP210x chip. Other drivers for other chips can likely be found through [Google](https://www.google.com).

#### Connect Raspberry Pi Mini UART to Computer Through USB

Follow instructions from the [Connect the Lead section of this document](https://cdn-learn.adafruit.com/downloads/pdf/adafruits-raspberry-pi-lesson-5-using-a-console-cable.pdf). If you have issues, note that I personally have purchased cables with the TXD and RXD wires reversed (with TXD green and RXD white). The TXD wire should go next to the GND wire, and hopefully your wires are colored as in the guide. Also, personally, I have always powered the Raspberry Pi through USB, not the console lead.

The following section of the guide gives instructions for connecting to a terminal emulator. Do this.

#### Login to Your Raspberry Pi Through the Console

After connecting to the terminal emulator, login to the Raspberry Pi with username `pi` and password `raspberry`. If everything has gone right, and you are ready to move on to the next chapter, you will see something that looks like:

![Raspberry Pi Console Login](images/rpi4_uart_login.png)

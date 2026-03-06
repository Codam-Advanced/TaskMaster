# TaskMaster

TaskMaster is a job management system that works similarly to [Supervisor](https://supervisord.org/). It consists of two executables: `taskmasterd` and `taskmasterctl`.

## Executables

- **taskmasterd**: A daemon that manages jobs.
- **taskmasterctl**: A client that connects to `taskmasterd` through a Unix socket.

## Dependencies

Before building TaskMaster, make sure you have the following system dependencies installed:

- **readline**: A library for command-line input editing.

## Used Libraries

TaskMaster includes the following libraries that are fetched through CMake:

- **Google's Protocol Buffers**: For data interchange between the daemon and client.
- **yaml-cpp**: A [YAML parser](https://github.com/jbeder/yaml-cpp.git) for the config file.

## Configuration

TaskMaster uses a YAML config file. Below is an example of such a configuration:

```yaml
jobs: 
  ls:
    cmd: "/usr/bin/ls"
    numprocs: 1
    umask: 077
    workingdir: /tmp
    autostart: false
    autorestart: false
    exitcodes: 0
    starttime: 1
    stopsignal: QUIT
    stoptime: 10
    stdout: /tmp/ls.out
    stderr: /tmp/ls.err
```

## Build Instructions

To build TaskMaster, follow these steps:

1. Create a build directory:
   ```bash
   mkdir build
   ```
2. Change into the build directory:
   ```bash
   cd build
   ```
3. Run CMake to configure the project:
   ```bash
   cmake ..
   ```
4. Build the project using Make:
   ```bash
   make install -j6
   ```

## Usage

To run TaskMaster, follow these steps:

1. Open two terminal windows.

2. In the first terminal, navigate to the project's `/bin` directory and start the `taskmasterd` daemon:
   ```bash
   cd ../bin
   ./taskmasterd
   ```

3. In the second terminal, navigate to the same `/bin` directory and run the `taskmasterctl` client. For example, to check the status of jobs:
   ```bash
   cd ../bin
   ./taskmasterctl
   taskmasterctl: status
   ```

Make sure the configuration file `taskconfig.yaml` is located at the root of the repository, as it will be utilized by `taskmasterd`.

## Commands

The following commands can be executed through `taskmasterctl`:

- **start**: Starts a job.
- **restart**: Restarts a job.
- **status [job]**: Sends the status of all jobs, or the provided job.
- **reload**: Reloads the config file.
- **terminate**: Terminates the daemon process and all jobs it manages.

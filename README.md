# Pancake: Frequency Smoothed Databases
Implementation of Pancake by Grubbs et al. in C++14.

This implementation of Pancake was done on Visual Studio in Windows 11, so it might not work on Linux. You may have to install Visual Studio to compile and run the code. Additionally, Windows does not have a very simple way of installing OpenSSL, which is a dependency of this program. I installed OpenSSL on Windows through Chocolatey; you may need to install Perl and NASM first to use it.

Perl: https://strawberryperl.com/
NASM: https://www.nasm.us/
Choco: https://chocolatey.org/
OpenSSL's Windows Installation Notes: https://github.com/openssl/openssl/blob/master/NOTES-WINDOWS.md

You should be able to run choco install openssl in an administrator terminal.

You may want to use YCSB to generate new workloads other than the ones provided in the workloads folder. YCSB and instructions on usage can be found at https://github.com/brianfrankcooper/YCSB. On Windows, there may be an issue running YCSB saying that %JAVA_HOME% is not defined correctly. YCSB requres Maven, which runs on Java. Maven can be found at https://maven.apache.org/install.html, though you can use Chocolatey to install it. 

If echo %JAVA_HOME% does not return a path to a Java JDK, set %JAVA_HOME% with setx /m JAVA_HOME "{insert path to JDK}". 

If you are running on Visual Studio, you will have to set up the linker to OpenSSL. In the Properties of a Visual Studio solution, go to C/C++ > General. In Additional Include Directories, add C:\Program Files\OpenSSL-Win64\include (or wherever the include directory is for your installaltion of OpenSSL). Go to Linker > General. In Additional Library Directories, add C:\Program Files\OpenSSL-Win64\lib (or wherever the lib directory is). Finally, in Linker > Input, in Additional Dependencies, add C:\Program Files\OpenSSL-Win64\lib\VC\x64\MT\libssl.lib and C:\Program Files\OpenSSL-Win64\lib\VC\x64\MT\libcrypto.lib (or wherever these lib files are located). 

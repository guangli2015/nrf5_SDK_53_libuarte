1. preparations

1.1 if you don't have Make, get it from https://github.com/xpack-dev-tools/windows-build-tools-xpack/releases. I use this one: xpack-windows-build-tools-4.2.1-2-win32-x64.zip
1.2 change components\toolchain\gcc\Makefile.windows. Replace the value with your gcc compiler location.
1.3 update modules\nrfx. You can get the latest files from NCS.
1.4 update integration\nrfx. You can get the latest files from NCS.

2. Build with gcc

2.1 enter folder: examples\peripheral\uart\pca10095\blank\armgcc and open cmd
2.2 enter 'make'
2.3 enter 'make flash'

3. Build with SES
3.1 open the project in examples\peripheral\uart\pca10095\blank\ses
3.2 build and download
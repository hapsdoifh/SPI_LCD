Modified the kernel and allow the spi_lcd module to be run on the compex WPJ253 board through a bitbanged spi connection
Modified the spi_lcd program to display a list of images

Works for linux 4.14.245

In the openwrt build environment, place the .patch files under /QCA9563OS/isc/target/linux/ar71xx/patches-4.14



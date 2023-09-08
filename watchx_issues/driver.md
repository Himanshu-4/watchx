### nerf oled driver issues 
- the oled driver is implemented through SPI DMA transfers and DMA cannot access flash so have to
copy the flash (frames buffer) data into ram buffers and then send it 
// SPDX-License-Identifier: GPL-2.0-only
/*
 * MAX14001/MAX14002 SPI ADC driver
 *
 * Copyright (c) 2025 Marilene Andrade Garcia <marilene.agarcia@gmail.com>
 *
 * Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/MAX14001-MAX14002.pdf
 */

#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/iio/iio.h>

/* MAX14001 registers definition */
#define MAX14001_REG_ADC				0x00
#define MAX14001_REG_FADC				0x01
#define MAX14001_REG_FLAGS				0x02
#define MAX14001_REG_FLTEN				0x03
#define MAX14001_REG_THL				0x04
#define MAX14001_REG_THU				0x05
#define MAX14001_REG_INRR				0x06
#define MAX14001_REG_INRT				0x07
#define MAX14001_REG_INRP				0x08
#define MAX14001_REG_CFG				0x09
#define MAX14001_REG_ENBL				0x0A
#define MAX14001_REG_ACT				0x0B
#define MAX14001_REG_WEN				0x0C

/* MAX14001 verification registers definition */
#define MAX14001_REG_VERIFICATION(x)	(0x10 + (x))
#define MAX14001_REG_FLTV				0x13
#define MAX14001_REG_THLV				0x14
#define MAX14001_REG_THUV				0x15
#define MAX14001_REG_INRRV				0x16
#define MAX14001_REG_INRTV				0x17
#define MAX14001_REG_INRPV				0x18
#define MAX14001_REG_CFGV				0x19
#define MAX14001_REG_ENBLV				0x1A

/* MAX14001_REG_FLAGS MASKS */
#define MAX14001_MASK_FLAGS_ADC			BIT(1)
#define MAX14001_MASK_FLAGS_INRD		BIT(2)
#define MAX14001_MASK_FLAGS_SPI			BIT(3)
#define MAX14001_MASK_FLAGS_COM			BIT(4)
#define MAX14001_MASK_FLAGS_CRCL		BIT(5)
#define MAX14001_MASK_FLAGS_CRCF		BIT(6)
#define MAX14001_MASK_FLAGS_FET			BIT(7)
#define MAX14001_MASK_FLAGS_MV			BIT(8)

/* MAX14001_REG_FLTEN MASKS */
#define MAX14001_MASK_FLTEN_DYEN		BIT(0)
#define MAX14001_MASK_FLTEN_EADC		BIT(1)
#define MAX14001_MASK_FLTEN_EINRD		BIT(2)
#define MAX14001_MASK_FLTEN_ESPI		BIT(3)
#define MAX14001_MASK_FLTEN_ECOM		BIT(4)
#define MAX14001_MASK_FLTEN_ECRCL		BIT(5)
#define MAX14001_MASK_FLTEN_ECRCF		BIT(6)
#define MAX14001_MASK_FLTEN_EFET		BIT(7)
#define MAX14001_MASK_FLTEN_EMV			BIT(8)

u16 max14001_reverse_uint16(u16 x) {
	u16 y= (u16) ((((x >> 0) & 1 ) << 15) |
				  (((x >> 1) & 1 ) << 14) |
				  (((x >> 2) & 1 ) << 13) |
				  (((x >> 3) & 1 ) << 12) |
				  (((x >> 4) & 1 ) << 11) |
				  (((x >> 5) & 1 ) << 10) |
				  (((x >> 6) & 1 ) <<  9) |
				  (((x >> 7) & 1 ) <<  8) |
				  (((x >> 8) & 1 ) <<  7) |
				  (((x >> 9) & 1 ) <<  6) |
				  (((x >> 10) & 1) <<  5) |
				  (((x >> 11) & 1) <<  4) |
				  (((x >> 12) & 1) <<  3) |
				  (((x >> 13) & 1) <<  2) |
				  (((x >> 14) & 1) <<  1) |
				  (((x >> 15) & 1) <<  0));
	return y;
}

static int max14001_read_raw(struct iio_dev *indio_dev,
				struct iio_chan_spec const *chan,
				int *val, int *val2, long mask)
{
	struct max14001_state *st = iio_priv(indio_dev);

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		pr_err("[Log Debug] max14001_read_raw: IIO_CHAN_INFO_RAW\n");
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SCALE:
		pr_err("[Log Debug] max14001_read_raw: IIO_CHAN_INFO_SCALE\n");
		return IIO_VAL_INT;
	}

	return -EINVAL;
}

static int max14001_write_raw(struct iio_dev *indio_dev,
				struct iio_chan_spec const *chan,
				int val, int val2, long mask)
{
	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		pr_err("[Log Debug] max14001_write_raw: IIO_CHAN_INFO_RAW\n");
		return 0;
	}

	return -EINVAL;
}

static const struct iio_info max14001_info = {
	.read_raw = max14001_read_raw,
	.write_raw = max14001_write_raw,
};

static const struct iio_chan_spec max14001_channel[] = {
	{
		.type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
					  BIT(IIO_CHAN_INFO_SCALE),
	}
};

static int max14001_probe(struct spi_device *spi)
{
	pr_err("[Log Debug] max14001_probe\n");

	struct iio_dev *indio_dev;
	indio_dev = devm_iio_device_alloc(&spi->dev, 0);
	if (!indio_dev)
		return -ENOMEM;

	indio_dev->name = "max14001"; //spi_get_device_id(spi)->name;
	indio_dev->channels = max14001_channel;
	indio_dev->num_channels = ARRAY_SIZE(max14001_channel);
	indio_dev->info = &max14001_info;

	return devm_iio_device_register(&spi->dev, indio_dev);
}

static struct spi_driver max14001_driver = {
	.driver = {
		.name = "max14001",
	},
	.probe = max14001_probe,
};
module_spi_driver(max14001_driver);

MODULE_AUTHOR("Marilene Andrade Garcia <marilene.agarcia@gmail.com>");
MODULE_DESCRIPTION("Analog Devices MAX14001/MAX14002 ADCs driver");
MODULE_LICENSE("GPL v2");
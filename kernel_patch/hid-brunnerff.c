// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Force Feedback HID patch for Brunner CLS-P
 *
 *  Copyright (c) 2023 Jules Noirant
 */

/*
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>

#include "usbhid/usbhid.h"
#include "hid-ids.h"

static int brunnerff_init(struct hid_device *hdev)
{
	struct list_head *report_list = &hdev->report_enum[HID_OUTPUT_REPORT].report_list;
	struct hid_report *report;
	int i;

	/*
	 * Access the device control report at ID 0x0c
	 */
	for(i = 0 ; i < 11 ; i++){
		report_list = report_list->next;
	}

	report = list_entry(report_list, struct hid_report, list);

	/*
	 * Disable the default autocenter spring effect that prevents other effects rendering
	 */
	report->field[0]->value[0] = 0x03;

	hid_hw_request(hdev, report, HID_REQ_SET_REPORT);

	return 0;
}

static int brunner_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret;

	/*
	 * The device has 2 Interfaces. Only the first one is used for the actual control.
	 */
	if (hdev->dev_rsize == 29) {
		return -ENODEV;
	}

	ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "parse failed\n");
		return ret;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret) {
		hid_err(hdev, "hw start failed\n");
		return ret;
	}

	ret = brunnerff_init(hdev);
	if (ret) {
		hid_err(hdev, "could not initialize the joystick\n");
		return ret;
	}

	return 0;
}

static void brunner_remove(struct hid_device *hdev)
{
	hid_hw_stop(hdev);
}

static const struct hid_device_id brunner_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_BRUNNER, USB_DEVICE_ID_BRUNNER_CLS_P_DIRECTX) },
	{ }
};
MODULE_DEVICE_TABLE(hid, brunner_devices);

static struct hid_driver brunner_driver = {
	.name = "brunner-ff",
	.id_table = brunner_devices,
	.probe = brunner_probe,
	.remove = brunner_remove,
};
module_hid_driver(brunner_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jules Noirant");
MODULE_DESCRIPTION("Force feedback patch for Brunner devices");

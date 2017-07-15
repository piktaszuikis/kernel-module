#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/gpio/consumer.h>
#include <linux/hrtimer.h>
#include <linux/sched.h>

/*
 * This time we will avoid global variables.
 * We gonna put our data in once nice structure and pass it around functions.
 */
struct mirksiukas_data {
	struct gpio_desc *ledas;
	struct hrtimer htimer;
	ktime_t period;
	u8 is_on;
};


/*
 * This function is called each second. It either turns on or turns off the LED.
 */
static enum hrtimer_restart on_pulse(struct hrtimer * timer)
{
	//This is a magical macro to get a struct from struct's property.
	//We are using it instead of platform_get_drvdata, because we don't have pdev here.
	struct mirksiukas_data *data = container_of(timer, struct mirksiukas_data, htimer);

	data->is_on = !data->is_on;
	gpiod_set_value(data->ledas, data->is_on);

	//Reschedule timer for the next tick
	hrtimer_forward_now(&data->htimer, data->period);
	return HRTIMER_RESTART;
}

/*
 * Almost the same as module_init, but not quite it. It is called when device is plugged in (in our case
 * the moment driver is loaded).
 */
static int mirksiukas_probe(struct platform_device *pdev)
{
	//We are using devm_* function, because we don't want to bother freeing memory on module
	//unloading. It will happen automagically.
	struct mirksiukas_data *data = devm_kzalloc(&pdev->dev, sizeof(struct mirksiukas_data), GFP_KERNEL);

	if(!data)
		return -ENOMEM;

	//We associate our data with pdev, so that we could retrieve our data from pdev anywhere we need.
	platform_set_drvdata(pdev, data);

	//Find a led-gpios in device-tree and use it's value to get GPIO pin.
	data->ledas = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
	if (IS_ERR(data->ledas)) {
		dev_err(&pdev->dev, "Nepavyko gauti GPIO. Arba nėra įrašo device-tree, arba GPIO jau užimtas.\n");
		return PTR_ERR(data->ledas);
	}

	//Timer initialization. We will blink each 1 second.
	data->period = ktime_set(1, 0); //seconds, nanoseconds
    hrtimer_init (&data->htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
    data->htimer.function = on_pulse;
    hrtimer_start(&data->htimer, data->period, HRTIMER_MODE_REL);

	return 0;
}

static int mirksiukas_remove(struct platform_device *pdev)
{
	struct mirksiukas_data *data = platform_get_drvdata(pdev);

	//hrtimer_cancel will wait till on_pulse finishes if it is executing and cancel the timer.
	hrtimer_cancel(&data->htimer);

	//We want to turn LED off during unloading.
	gpiod_set_value(data->ledas, 0);

	return 0;
}

//Property 'compatible' will be used to find a configuration node in the device-tree
static const struct of_device_id match[] = {
	{ .compatible = "mirksiukas", },
	{},
};

/*
 * Instead of module_init and module_exit we will be using module_platform_driver.
 * This function takes 'struct platform_driver' as a parameter.
 * We use this struct to describe our module: initialization/unloading functions,
 * driver name and device-tree node search criteria.
 */
static struct platform_driver mirksiukas_driver = {
	.probe = mirksiukas_probe,
	.remove = mirksiukas_remove,
	.driver = {
		.name = "mano-mirksiukas",
		.of_match_table = match,
	},
};


module_platform_driver(mirksiukas_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Piktas Zuikis");
MODULE_DESCRIPTION("Linux kernel driver for blinking a LED via GPIO.");

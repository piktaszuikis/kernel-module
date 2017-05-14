#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>

static unsigned int busnum;
module_param(busnum, uint, 0);
MODULE_PARM_DESC(busnum, "SPI bus number (magistralės nr.) (default=0)");

static struct spi_device *spi_kanalas;

static int __init spi_init(void)
{
	int ret;
	uint8_t komanda_kuria_siusime_irenginiui = 0b11010011;
	struct spi_master *masteris;

	//Informacija apie įrenginį, kuris bus prijungtas (slave)
	struct spi_board_info spi_irenginys = {
		.modalias = "spi-testas",
		.max_speed_hz = 1000000,
		.bus_num = busnum,
		.mode = SPI_MODE_0,
	};

	//Gauname masterį, kuris bendraus su mūsų įrenginiu
	printk("spi-testas: Gauname masterį, kuris bendraus su mūsų įrenginiu (magistralės nr.: %u).\n", spi_irenginys.bus_num);
	masteris = spi_busnum_to_master(spi_irenginys.bus_num);
	if(!masteris)
	{
		printk(KERN_ERR "spi-testas: nerastas 'master'. Greičiausiai toks įrenginys neegzistuoja.\n");
		return -ENODEV;
	}

	//Sukuriame kanalą, per kurį masteris bendraus su įrenginiu
	spi_kanalas = spi_new_device( masteris, &spi_irenginys );
	if(!spi_kanalas)
	{
		printk(KERN_ERR "spi-testas: nepavyko sukurti kanalo bendravimui su įrenginiu.\n");
		return -ENODEV;
	}

	//Inicializuojame šį kanalą (kompiuteriuko masterio  driveriai priskiria numatytasias reikšmes)
	ret = spi_setup(spi_kanalas);
	if(ret)
	{
		printk(KERN_ERR "spi-testas: nepavyko inicializuoti kanalo bendravimui su įrenginiu.\n");
		spi_unregister_device(spi_kanalas);
		return -ENODEV;
	}

	ret = spi_write(spi_kanalas, &komanda_kuria_siusime_irenginiui, sizeof(uint8_t));
	if(ret)
	{
		printk(KERN_ERR "spi-testas: klaida siunčiant duomenis.\n");
		spi_unregister_device(spi_kanalas);
		return ret;
	}

	printk("spi-testas: duomenys nusiųsti.\n");

	return 0;
}


static void __exit spi_exit(void)
{
	spi_unregister_device(spi_kanalas);
}

module_init(spi_init);
module_exit(spi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Piktas Zuikis");
MODULE_DESCRIPTION("SPI duomenų siuntimo pavyzdys.");

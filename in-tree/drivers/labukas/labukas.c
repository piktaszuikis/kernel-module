#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
static int __init inicializuoti_moduli(void)
{
	printk(KERN_INFO "Sveikas, pasauli!\n");
	/* 
	 * Reikia gražinti 0, antraip kernelis galvos, kad inicializavimas nulūžo.
	 * Jeigu gražinsime >0 kernelis labai supyks, bet vistiek paleis modulį.
	 * Jeigu gražinsime <0 kernelis nepaleis modulio ir atspausdins klaidos žinutę pagal gražinamą klaidos kodą.
	 */
	return 0;
}
static void __exit atlaisvinti_moduli(void)
{
	printk(KERN_INFO "Sudie, pasauli!\n");
}
module_init(inicializuoti_moduli);
module_exit(atlaisvinti_moduli);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Piktas Zuikis");
MODULE_DESCRIPTION("Paprastas 'Sveikas, pasauli!' modulis.");

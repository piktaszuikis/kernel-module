Modified device-tree for the blinker example. Added a branch 'ledo_lempute' with led-gpios leaf:

`led-gpios = <&pio 8 11 GPIO_ACTIVE_HIGH>;`


We are using *pio* gpio-controller and [PI11 GPIO pin](https://linux-sunxi.org/LinkSprite_pcDuino3#Expansion_headers) (I is 8-th letter counting from 0, hence `&pio 8 11`). When we set GPIO output to 1 in our code,  we want it to output high signal on the physical pin too, therefore we use GPIO_ACTIVE_HIGH. We could use GPIO_ACTIVE_LOW to achieve a reverse effect.

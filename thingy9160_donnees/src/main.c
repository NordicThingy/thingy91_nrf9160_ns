//Bibliothèques nécessaires au fonctionnement du programme
#include <stdio.h>
#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#if defined(CONFIG_ADP536X)
#include <adp536x.h>
#endif

#define SLEEP_TIME_MS   1000 //Temps de pause entre chaque loop
#define SEUIL 15.0  // Seuil de choc en m/s²

#define LED0_NODE DT_ALIAS(led0) //L'identifiant du nœud devicetree pour l'alias « led0 ».

#define ACCEL_ALIAS(i) DT_ALIAS(_CONCAT(accel, i))
#define ACCELEROMETER_DEVICE(i, _)                                                                 \
	IF_ENABLED(DT_NODE_EXISTS(ACCEL_ALIAS(i)), (DEVICE_DT_GET(ACCEL_ALIAS(i)),))

static const struct device *const sensors[] = {LISTIFY(10, ACCELEROMETER_DEVICE, ())}; //Supporte jusqu'à 10 accéléromètres

static const enum sensor_channel channels[] = { //Nom des axes de mesure d'accélérations 
	SENSOR_CHAN_ACCEL_X,
	SENSOR_CHAN_ACCEL_Y,
	SENSOR_CHAN_ACCEL_Z,
};

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);  //Déclaration de la broche gpio au nom led pour la programmer

static int shock_counter = 0; // Compteur de chocs

//fonction de détection des chocs si l'accélération mesuré est supérieur au seuil
void detectShock(float acc_x, float acc_y, float acc_z) {
    if (fabs(acc_x) > SEUIL || fabs(acc_y) > SEUIL || fabs(acc_z) > SEUIL) {
        shock_counter++;
        printk("Attention, choc detecte ! Compteur de chocs : %d\n", shock_counter);
    }
}

//fonction de détection de l'orientation en fonction des valeurs d'accélération mesuré
void detectOrientation(float acc_x, float acc_y, float acc_z){
	if (acc_z >= -12 && acc_z <= -8 && acc_x >= -1 && acc_x <= 1 && acc_y >= -1 && acc_y <= 1) {
        printf("Orientation: a plat (0°)\n");
    } else if (acc_x >= -12 && acc_x <= -8 && acc_y >= -1 && acc_y <= 1 && acc_z >= -1 && acc_z <= 1) {
        printf("Orientation: verticale (90°)\n");
    } else if (acc_z >= 8 && acc_z <= 12 && acc_x >= -1 && acc_x <= 1 && acc_y >= -1 && acc_y <= 1) {
        printf("Orientation : à l'envers (180°)\n");
    } else if (acc_y >= -12 && acc_y <= -8 && acc_x >= -1 && acc_x <= 1 && acc_z >= -1 && acc_z <= 1){
        printf("Orientation: verticale (90°)\n");
    } else if (acc_x >= 8 && acc_x <= 12 && acc_y >= -1 && acc_y <= 1 && acc_z >= -1 && acc_z <= 1){
        printf("Orientation: verticale (90°)\n");
    } else if (acc_y >= 8 && acc_y <= 12 && acc_x >= -1 && acc_x <= 1 && acc_z >= -1 && acc_z <= 1){
        printf("Orientation: verticale (90°)\n");
    } else if (acc_y >= -9 && acc_y <= -7 && acc_x >= -1 && acc_x <= 1 && acc_z >= -5 && acc_z <= -3){
        printf("Orientation: verticale à l'endroit (45°)\n");
    } else if (acc_y >= -9 && acc_y <= -7 && acc_x >= -1 && acc_x <= 1 && acc_z >= 4 && acc_z <= 6){
        printf("Orientation: verticale à l'envers(135°)\n");
	} else if (acc_y >= -1 && acc_y <= 1 && acc_x >= 7 && acc_x <= 10 && acc_z >= -5 && acc_z <= -3){
        printf("Orientation: verticale à l'endroit(45°)\n");
	} else if (acc_y >= -1 && acc_y <= 1 && acc_x >= -9 && acc_x <= -7 && acc_z >= 4 && acc_z <= 6){
        printf("Orientation: verticale à l'envers(135°)\n");
	} else if (acc_y >= 7 && acc_y <= 9 && acc_x >= -1 && acc_x <= 1 && acc_z >= -6 && acc_z <= -3){
        printf("Orientation: verticale à l'endroit(45°)\n");
	} else if (acc_y >= 7 && acc_y <= 9 && acc_x >= -1 && acc_x <= 1 && acc_z >= 5 && acc_z <= 7){
        printf("Orientation: verticale à l'envers(135°)\n");
	} else if (acc_y >= -1 && acc_y <= 1 && acc_x >= -9 && acc_x <= -7 && acc_z >= -6 && acc_z <= -4){
        printf("Orientation: verticale à l'endroit(45°)\n");
	} else if (acc_y >= -2 && acc_y <= 1 && acc_x >= 7 && acc_x <= 9 && acc_z >= 4 && acc_z <= 6){
        printf("Orientation: verticale à l'envers(135°)\n");
	} else {
		printf("Orientation: non défini\n");
	}
}

//fonction d'affichage des valeurs des accéléromètres
static int print_accels(const struct device *dev1)
{
	int ret1;
	struct sensor_value accel[3]; //déclaration des trois accéléromètres

	ret1 = sensor_sample_fetch(dev1); //association de ret1 à la détection de l'accéléromètre
	if (ret1 < 0) {
		printk("%s: sensor_sample_fetch() failed: %d\n", dev1->name, ret1);
		return ret1;
	}

	//fonction de mesure et d'affichage des accéléromètre et vérification de la bonne réception de données
	for (size_t i = 0; i < ARRAY_SIZE(channels); i++) {
		ret1 = sensor_channel_get(dev1, channels[i], &accel[i]);
		if (ret1 < 0) {
			printk("%s: sensor_channel_get(%c) failed: %d\n", dev1->name, 'X' + i, ret1);
			return ret1;
		}
	}

	//Affichage des données mesurées
	printk("x = %12.6f m/s^2\n", sensor_value_to_double(&accel[0]));
	printk("y = %12.6f m/s^2\n", sensor_value_to_double(&accel[1]));
	printk("z = %12.6f m/s^2\n", sensor_value_to_double(&accel[2]));
	// Détecter le choc et l'orientation
	detectShock(sensor_value_to_double(&accel[0]), sensor_value_to_double(&accel[1]), sensor_value_to_double(&accel[2]));
	detectOrientation(sensor_value_to_double(&accel[0]), sensor_value_to_double(&accel[1]), sensor_value_to_double(&accel[2]));
	return 0;
}


//fonction principale
int main(void)
{
	const struct device *const dev_bme = DEVICE_DT_GET_ONE(bosch_bme680); //Détection de la position du capteur environnementale
	struct sensor_value temp, press, humidity; //variable du bme280

    int ret;
	int counter = 0;  // Compteur pour mesurer les secondes

    if (!gpio_is_ready_dt(&led)) { //Si la broche d ela led n'est pas détecté
        return 0;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE); //Si la broche ne renvoie rien
    if (ret < 0) {
        return 0;
    }

	if (!device_is_ready(dev_bme)) { //Détection de la présence du capteur environnement
		printk("sensor: device not ready.\n");
		return 0;
	}

	printf("Device %p name is %s\n", dev_bme, dev_bme->name); //Affichage du nom du apteur

	int ret1;

	for (size_t i = 0; i < ARRAY_SIZE(sensors); i++) { //Détection de la présence de l'accéléromètre
		if (!device_is_ready(sensors[i])) {
			printk("sensor: device %s not ready.\n", sensors[i]->name);
			return 0;
		}
	}

#ifndef CONFIG_COVERAGE
	while (1) {
#else
	for (int i = 0; i < 5; i++) {
#endif
		if (counter % 30 == 0) {  // Toutes les 10 secondes
			gpio_pin_set_dt(&led, 1);  // Allume la LED
			k_msleep(1000);            // Attend 1 seconde
			gpio_pin_set_dt(&led, 0);  // Éteint la LED
		}

		//Appel de la fonction de mesure et d'affichage de l'accélération 3 axes
		printf("Données accélérations:\n"); 
		for (size_t i = 0; i < ARRAY_SIZE(sensors); i++) {
			ret1 = print_accels(sensors[i]);
			if (ret1 < 0) {
				return 0;
			}
		}
	

		//Mesure des données environnementales du bme280
		sensor_sample_fetch(dev_bme);
		sensor_channel_get(dev_bme, SENSOR_CHAN_AMBIENT_TEMP, &temp);
		sensor_channel_get(dev_bme, SENSOR_CHAN_PRESS, &press);
		sensor_channel_get(dev_bme, SENSOR_CHAN_HUMIDITY, &humidity);

		//Affichage des données environnmentales
		printf("Données environnementales:\n");
		printf("T: %d.%06d °C\n", temp.val1, temp.val2);
		printf("P: %d.%06d kPa\n", press.val1, press.val2);
		printf("H: %d.%06d %RH\n", humidity.val1, humidity.val2);
		printf("\n");
		
		counter++;  // Incrémente le compteur de secondes				
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}

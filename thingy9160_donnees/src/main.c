// Inclusion des bibliothèques standard et Zephyr nécessaires
#include <stdio.h>      // Pour l'affichage des informations (printf, printk)
#include <stdlib.h>     // Pour les opérations générales (ex: malloc, free)

#include <zephyr/device.h>      // Gestion des périphériques
#include <zephyr/drivers/sensor.h>  // Accès aux capteurs du Thingy:91
#include <zephyr/sys/util_macro.h>  // Macros utiles du système Zephyr
#include <zephyr/kernel.h>          // Fonctions du noyau Zephyr (ex: gestion des tâches)
#include <zephyr/drivers/gpio.h>    // Gestion des GPIO (LED, boutons, ...)
#include <zephyr/sys/printk.h>      // Fonction d'affichage printk

#if defined(CONFIG_ADP536X)  // Vérifie si la configuration pour ADP536X est activée
#include <adp536x.h>  // Inclusion du pilote pour ce composant (gestion de l'alimentation)
#endif

// Définition de constantes
#define SLEEP_TIME_MS   1000  // Temps d'attente en millisecondes entre chaque itération
#define SEUIL 15.0  // Seuil d'accélération en m/s² pour détecter un choc

// Définition de l'alias pour la LED0 dans le fichier Devicetree
#define LED0_NODE DT_ALIAS(led0) 

// Macros pour accéder aux accéléromètres définis dans Devicetree
#define ACCEL_ALIAS(i) DT_ALIAS(_CONCAT(accel, i))
#define ACCELEROMETER_DEVICE(i, _) \
	IF_ENABLED(DT_NODE_EXISTS(ACCEL_ALIAS(i)), (DEVICE_DT_GET(ACCEL_ALIAS(i)),))

// Tableau des capteurs accélérométriques (jusqu'à 10 capteurs maximum)
static const struct device *const sensors[] = {LISTIFY(10, ACCELEROMETER_DEVICE, ())};

// Liste des axes d'accélération à mesurer (X, Y, Z)
static const enum sensor_channel channels[] = {
	SENSOR_CHAN_ACCEL_X,
	SENSOR_CHAN_ACCEL_Y,
	SENSOR_CHAN_ACCEL_Z,
};

// Configuration de la LED pour pouvoir la contrôler via GPIO
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);  

// Variable globale pour compter le nombre de chocs détectés
static int shock_counter = 0; 

// Fonction de détection des chocs en analysant les accélérations mesurées
void detectShock(float acc_x, float acc_y, float acc_z) {
    if (fabs(acc_x) > SEUIL || fabs(acc_y) > SEUIL || fabs(acc_z) > SEUIL) {
        shock_counter++;  // Incrémentation du compteur de chocs
        printk("Attention, choc détecté ! Compteur de chocs : %d\n", shock_counter);
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

// Fonction qui récupère et affiche les valeurs d'accélération d'un capteur
static int print_accels(const struct device *dev1)
{
	int ret1;
	struct sensor_value accel[3]; // Stocke les valeurs des 3 axes d'accélération

	// Récupération des données du capteur
	ret1 = sensor_sample_fetch(dev1); 
	if (ret1 < 0) {
		printk("%s: Erreur lors de la lecture des données du capteur: %d\n", dev1->name, ret1);
		return ret1;
	}

	// Lecture des valeurs d'accélération pour les axes X, Y et Z
	for (size_t i = 0; i < ARRAY_SIZE(channels); i++) {
		ret1 = sensor_channel_get(dev1, channels[i], &accel[i]);
		if (ret1 < 0) {
			printk("%s: Erreur lors de la lecture de l'axe %c : %d\n", dev1->name, 'X' + i, ret1);
			return ret1;
		}
	}

	// Affichage des valeurs d'accélération
	printk("x = %12.6f m/s^2\n", sensor_value_to_double(&accel[0]));
	printk("y = %12.6f m/s^2\n", sensor_value_to_double(&accel[1]));
	printk("z = %12.6f m/s^2\n", sensor_value_to_double(&accel[2]));

	// Détection de choc et d'orientation
	detectShock(sensor_value_to_double(&accel[0]), sensor_value_to_double(&accel[1]), sensor_value_to_double(&accel[2]));
	detectOrientation(sensor_value_to_double(&accel[0]), sensor_value_to_double(&accel[1]), sensor_value_to_double(&accel[2]));

	return 0;
}

// Fonction principale du programme
int main(void)
{
	// Détection du capteur environnemental Bosch BME680
	const struct device *const dev_bme = DEVICE_DT_GET_ONE(bosch_bme680);
	struct sensor_value temp, press, humidity; // Variables pour stocker les valeurs

    int ret;
	int counter = 0;  // Compteur de secondes

    // Vérification si la LED est disponible
    if (!gpio_is_ready_dt(&led)) {
        return 0;
    }

    // Configuration de la LED en sortie
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return 0;
    }

	// Vérification si le capteur BME680 est prêt
	if (!device_is_ready(dev_bme)) {
		printk("Capteur BME680 non détecté.\n");
		return 0;
	}

	printf("Capteur environnemental détecté: %s\n", dev_bme->name);

	// Vérification de la présence des accéléromètres
	for (size_t i = 0; i < ARRAY_SIZE(sensors); i++) {
		if (!device_is_ready(sensors[i])) {
			printk("Capteur %s non détecté.\n", sensors[i]->name);
			return 0;
		}
	}

	// Boucle principale
	while (1) {
		// Clignotement de la LED toutes les 30 secondes
		if (counter % 30 == 0) {  
			gpio_pin_set_dt(&led, 1);
			k_msleep(1000);
			gpio_pin_set_dt(&led, 0);
		}

		// Affichage des valeurs des accéléromètres
		printf("Données accélérations:\n"); 
		for (size_t i = 0; i < ARRAY_SIZE(sensors); i++) {
			if (print_accels(sensors[i]) < 0) {
				return 0;
			}
		}
	
		// Lecture des données environnementales
		sensor_sample_fetch(dev_bme);
		sensor_channel_get(dev_bme, SENSOR_CHAN_AMBIENT_TEMP, &temp);
		sensor_channel_get(dev_bme, SENSOR_CHAN_PRESS, &press);
		sensor_channel_get(dev_bme, SENSOR_CHAN_HUMIDITY, &humidity);

		// Affichage des valeurs
		printf("Données environnementales:\n");
		printf("Température: %d.%06d °C\n", temp.val1, temp.val2);
		printf("Pression: %d.%06d kPa\n", press.val1, press.val2);
		printf("Humidité: %d.%06d %%RH\n", humidity.val1, humidity.val2);
		printf("\n");

		// Incrémentation du compteur et attente avant la prochaine boucle
		counter++;
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}

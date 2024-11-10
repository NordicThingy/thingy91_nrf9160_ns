Détection de Chocs et Orientation avec Accéléromètres et Capteur Environnemental sur Thingy:91

Ce projet permet de détecter les chocs et l'orientation du Thingy:91 de Nordic Semiconductor en utilisant des accéléromètres et un capteur environnemental. Le programme surveille en continu les données des capteurs et allume une LED en fonction des événements détectés. Le code est compatible avec le SDK nRF Connect.


**<u>Matériel requis:<u>**
- Thingy:91 de Nordic Semiconductor
- nRF Connect for Desktop
- Visual Studio Code avec l'extension nRF Connect for VS Code
- Putty
- Câble micro-USB


Installation et Configuration:
Pour exécuter ce projet, vous devez installer et configurer le nRF Connect for Desktop et Visual Studio Code (VS Code). 
Note : Assurez-vous que la chaîne d'outils (Toolchain) et le SDK sont installés sur le disque C:\ de l'ordinateur, dans un dossier dédié. Le dossier contenant le code pour le Thingy:91 doit également être placé dans un dossier de programmation sur le disque C:\.
Suivez les étapes ci-dessous pour la configuration :
1. Installation des outils en ligne de commande nRF
- Téléchargez et installez les nRF Command Line Tools depuis le lien suivant : https://www.nordicsemi.com/Products/Development-tools/nRF-Command-Line-Tools/Download.
- Les nRF Command Line Tools incluent nrfjprog, un outil essentiel pour flasher le firmware sur vos kits de développement Nordic.

2. Installation de Visual Studio Code
- Rendez-vous sur https://code.visualstudio.com/download et installez la version de VS Code correspondant à votre système d'exploitation.

4. Installation du nRF Connect Extension Pack dans VS Code
- Dans la barre d'activités de VS Code, cliquez sur l'icône Extensions.
- Recherchez nRF Connect for VS Code Extension Pack, puis cliquez sur Installer.

Le nRF Connect Extension Pack pour VS Code permet de :
  - Développer, créer, déboguer et déployer des applications embarquées basées sur le SDK nRF Connect.
  - Interagir avec le compilateur, l'éditeur de liens, le système de construction complet, un débogueur compatible RTOS, et le SDK nRF Connect.
  - Utiliser un éditeur visuel pour les fichiers Devicetree et un terminal série intégré.

4. Installation de la chaîne d'outils (Toolchain) sur VS Code
- Lors du premier lancement de l'extension nRF Connect for VS Code, vous serez invité à installer une Toolchain. Si aucune n'est détectée, procédez ainsi :
    - Cliquez sur Installer la Toolchain.
    - Choisissez une version compatible avec la version du SDK nRF Connect que vous utiliserez (la dernière version recommandée).
    - L'installation peut prendre quelques minutes.

5. Installation du SDK nRF Connect
Dans nRF Connect pour VS Code, cliquez sur Gérer le SDK.
Dans cette section, vous pourrez installer, désinstaller et sélectionner la version active du SDK.
Cliquez sur Installer le SDK pour afficher les versions disponibles et choisir celle que vous souhaitez utiliser.
Une fois ces étapes terminées, votre environnement est prêt pour le développement avec le SDK nRF Connect et Visual Studio Code.


Utilisation du code:
1. Chargement du code
- Ouvrir VS Code
- Télécharger et décompresser le code, puis ouvrez le dossier contenant le code dans VS Code.

2. Build du code :
- Dans l'extension nRF Connect dans la barre d'activité, cliquez sur Add Build Configuration.
- Dans Board Target, choisissez thingy91_nrf9160_ns.
- Dans Base Devicetree overlays, ajoutez et sélectionnez le fichier thingy91_nrf9160_ns.overlay.
    - Le Base Devicetree overlays liste les superpositions pour l’arborescence des périphériques qui peuvent être ajoutées ou modifiées pour décrire le matériel.
- Cliquez sur Build Configuration et attendez la fin du processus de compilation.

3. Envoi du code au Thingy:91
- Connectez la carte Thingy:91 à votre ordinateur via un câble micro-USB.
- Allumez la carte en appuyant sur le bouton SW3 puis en activant l'interrupteur ON/OFF.
![image](https://github.com/user-attachments/assets/94f3a7b5-c8cf-49e6-ae4f-acffe8581d2b)
- Ouvrez nRF Connect for Desktop, puis l'outil Programmer (à installer si nécessaire).
- Cliquez sur Select a Device et sélectionnez le Thingy:91 dans la liste.
- Dans Add File, cliquez sur Browse, allez dans le dossier du code > build/zephyr, et sélectionnez app_signed.hex.
- Cliquez ensuite sur Write, puis Write à nouveau dans la fenêtre de confirmation pour flasher le code.

4. Récupération des données avec PuTTY
- Installez PuTTY.
- Ouvrez PuTTY et configurez-le pour accéder au port COM du Thingy:91
- Pour identifier le port COM :
    - Appuyez sur Windows + X et ouvrez le Gestionnaire de périphériques.
    - Dans la fenêtre du Gestionnaire de périphériques, recherchez Ports (COM et LPT) et repérez le port associé au Thingy:91 (généralement COM5).
- Revenez à PuTTY et allez dans la section Session.
- Dans Specify the destination you want to connect to :
    - Sélectionnez Serial.
    - Dans Serial Line, entrez COMX (où X correspond au numéro de port COM identifié).
    - Définissez la vitesse (Speed) à 115200.
- Cliquez sur Open pour ouvrir la connexion série.
- Optionnel : Enregistrement des données mesurées
    - Dans PuTTY, allez à Session > Logging.
    - Sélectionnez All Session Output.
    - Dans Log File name, choisissez l’emplacement et le nom pour enregistrer les données.
- Cliquez sur Open pour lancer l’enregistrement des données dans le fichier log.

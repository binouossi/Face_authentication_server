# Présentation

Ce logiciel est la partie serveur d'un systeme de logiciel chargé de s'occuper de la reconnaissance faciale sur un controleur de domaine. Il est destiné uniquement aux environnements LINUX eet s'install de preferencr sur le controleur de domaine.

# Dépendance

* libopencv-dev (2.4.1)
* liqt4-dev

# Installation

1. Pour l'installation de ce logiciel dans un premier temps, [telecharger la source du logiciel](https://github.com/binouossi/Face_authentication_server).

2. Creer un dossier de compilation dans la source telechargée, et s'y placé.  

	mkdir build  
	cd buid  

3. Executer les commandes:  

	cmake -D CMAKE_INSTALL_PREFIX=/usr ..  
	make  
	make install  

## Configuration

Comme configuration, l'ajouter dans le init.d pour permettre son demarrage au lancement du systeme. Pour le faire vous pouvez utiliser la commande **update-rc.d**. Pour plus d'information, consulter son manuel.

# Auteur
TACOLODJOU Emmanuel  
Maintainer  
tacolodjounini@gmail.com  

# LICENCE

Ce logiciel est protege par la licence GPLv3

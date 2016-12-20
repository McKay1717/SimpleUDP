#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define BUFLEN 512

void die(char *s) {
	perror(s);
	exit(1);
}

int main(int argc, char *argv[]) {
	struct sockaddr_in si_me, si_other;
	struct hostent *he;
	struct in_addr ipv4addr;

	int s = sizeof(si_other);
	socklen_t slen;
	int recv_len;
	char buf[BUFLEN] ="";
	int port = 8888;

	//Verification des args
	if (argc != 2) {
		printf("Usage: %s port_serveur \n", argv[0]);
		exit(1);
	} else {
		port = atoi(argv[1]);

	}

	//Creation de la socket UDP
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		die("Impossible de créer la socket");
	}

	//Initialisation des structures
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//Ecoute de la socket sur le port
	if (bind(s, (struct sockaddr*) &si_me, sizeof(si_me)) == -1) {
		die("Ecoute Impossible");
	}

	//Boucle d'écoute
	while (1) {
		fflush(stdout);

		//Attente de données
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0,
				(struct sockaddr *) &si_other, &slen)) == -1) {
			die("Reception Impossible");
		}

		//Reverse lookup du client
		inet_pton(AF_INET, inet_ntoa(si_other.sin_addr), &ipv4addr);
		he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
		//Affichage des informations du client
		printf("CLIENT: %s:%d (%s)\n", inet_ntoa(si_other.sin_addr),
				ntohs(si_other.sin_port),  he->h_name);

		//Création de la réponse
		char *tmp = strdup(buf);

		strcpy(buf, "Bonjour ");
		strcat(buf, tmp);
		free(tmp);
		//Envoie de la reponse
		if (sendto(s, buf, sizeof(buf), 0, (struct sockaddr*) &si_other, slen)
				== -1) {
			die("Impossible de répondre au client");
		}
		//Vidage du buffer
		memset(buf,0,BUFLEN);

	}
//fermeture de la socket
	close(s);
	return 0;
}


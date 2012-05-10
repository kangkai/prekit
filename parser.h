#ifndef __PARSER_H
#define __PARSER_H

/*
 * config file has the below format
 * VN_IFWI =, # IFWI file
 * VN_FWR_DnX =, # FWR DnX file
 * NORMALOS =, # stitched normalos
 * PREOS =, # stitched preos
 * PLATFORM =, # platform
 * DATA =, (optional)
 * CSA =, (optional)
 */
struct config {
	char ifwi[PATH_MAX];
	char fwr_dnx[PATH_MAX];
	char boot[PATH_MAX];
	char preos[PATH_MAX];
	char platform[PATH_MAX];
	char data[PATH_MAX];
	char csa[PATH_MAX];
};

void display_config(struct config conf);
int parse_config(char *stream, size_t size, struct config *conf, char *ver);

#endif

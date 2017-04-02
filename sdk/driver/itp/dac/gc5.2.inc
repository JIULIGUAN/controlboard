/* factor = 5.200000, gamma = 0.192308 */
const unsigned char wm8960_perc_to_reg_table[] = {
	0x30, /* 000%, -73dB */
	0x50, /* 001%, -41dB */
	0x55, /* 002%, -36dB */
	0x58, /* 003%, -33dB */
	0x5b, /* 004%, -30dB */
	0x5c, /* 005%, -29dB */
	0x5e, /* 006%, -27dB */
	0x5f, /* 007%, -26dB */
	0x61, /* 008%, -24dB */
	0x62, /* 009%, -23dB */
	0x63, /* 010%, -22dB */
	0x64, /* 011%, -21dB */
	0x65, /* 012%, -20dB */
	0x66, /* 013%, -19dB */
	0x66, /* 014%, -19dB */
	0x67, /* 015%, -18dB */
	0x68, /* 016%, -17dB */
	0x68, /* 017%, -17dB */
	0x69, /* 018%, -16dB */
	0x6a, /* 019%, -15dB */
	0x6a, /* 020%, -15dB */
	0x6b, /* 021%, -14dB */
	0x6b, /* 022%, -14dB */
	0x6c, /* 023%, -13dB */
	0x6c, /* 024%, -13dB */
	0x6d, /* 025%, -12dB */
	0x6d, /* 026%, -12dB */
	0x6e, /* 027%, -11dB */
	0x6e, /* 028%, -11dB */
	0x6f, /* 029%, -10dB */
	0x6f, /* 030%, -10dB */
	0x6f, /* 031%, -10dB */
	0x70, /* 032%, -9dB */
	0x70, /* 033%, -9dB */
	0x71, /* 034%, -8dB */
	0x71, /* 035%, -8dB */
	0x71, /* 036%, -8dB */
	0x72, /* 037%, -7dB */
	0x72, /* 038%, -7dB */
	0x72, /* 039%, -7dB */
	0x73, /* 040%, -6dB */
	0x73, /* 041%, -6dB */
	0x73, /* 042%, -6dB */
	0x74, /* 043%, -5dB */
	0x74, /* 044%, -5dB */
	0x74, /* 045%, -5dB */
	0x74, /* 046%, -5dB */
	0x75, /* 047%, -4dB */
	0x75, /* 048%, -4dB */
	0x75, /* 049%, -4dB */
	0x76, /* 050%, -3dB */
	0x76, /* 051%, -3dB */
	0x76, /* 052%, -3dB */
	0x76, /* 053%, -3dB */
	0x77, /* 054%, -2dB */
	0x77, /* 055%, -2dB */
	0x77, /* 056%, -2dB */
	0x77, /* 057%, -2dB */
	0x78, /* 058%, -1dB */
	0x78, /* 059%, -1dB */
	0x78, /* 060%, -1dB */
	0x78, /* 061%, -1dB */
	0x78, /* 062%, -1dB */
	0x79, /* 063%, +0dB */
	0x79, /* 064%, +0dB */
	0x79, /* 065%, +0dB */
	0x79, /* 066%, +0dB */
	0x7a, /* 067%, +1dB */
	0x7a, /* 068%, +1dB */
	0x7a, /* 069%, +1dB */
	0x7a, /* 070%, +1dB */
	0x7a, /* 071%, +1dB */
	0x7b, /* 072%, +2dB */
	0x7b, /* 073%, +2dB */
	0x7b, /* 074%, +2dB */
	0x7b, /* 075%, +2dB */
	0x7b, /* 076%, +2dB */
	0x7c, /* 077%, +3dB */
	0x7c, /* 078%, +3dB */
	0x7c, /* 079%, +3dB */
	0x7c, /* 080%, +3dB */
	0x7c, /* 081%, +3dB */
	0x7d, /* 082%, +4dB */
	0x7d, /* 083%, +4dB */
	0x7d, /* 084%, +4dB */
	0x7d, /* 085%, +4dB */
	0x7d, /* 086%, +4dB */
	0x7d, /* 087%, +4dB */
	0x7e, /* 088%, +5dB */
	0x7e, /* 089%, +5dB */
	0x7e, /* 090%, +5dB */
	0x7e, /* 091%, +5dB */
	0x7e, /* 092%, +5dB */
	0x7e, /* 093%, +5dB */
	0x7f, /* 094%, +6dB */
	0x7f, /* 095%, +6dB */
	0x7f, /* 096%, +6dB */
	0x7f, /* 097%, +6dB */
	0x7f, /* 098%, +6dB */
	0x7f, /* 099%, +6dB */
};

const int wm8960_perc_to_reg_tableSize = sizeof(wm8960_perc_to_reg_table);

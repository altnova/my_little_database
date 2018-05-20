//! \file str.c \brief string utilities
#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "trc.h"
#include "str.h"

#ifdef RUN_TESTS_STR

#define FTI_TOKEN_DELIM " \\%$`^0123456789#@_?~;&/\\,!|+-.:()[]{}<>*=\"\t\n\r'"

I main() {
	LOG("str_test");
	S str = "?a b cd. e?";
	I l = scnt(str);

	O("(%s)=%d\n", str, l);

	// tokenize forwards
	stok(str, l, FTI_TOKEN_DELIM, 0,
		O("len=%2ld, pos=%2ld, tok=", tok_len, tok_pos);
		BYTES_AS_STR(tok, tok_len);
		O("\n");
	)
	O("=======================\n");

	// tokenize backwards
	stok(str, l, FTI_TOKEN_DELIM, 1,
		O("len=%2ld, pos=%2ld, tok=", tok_len, tok_pos);
		BYTES_AS_STR(tok, tok_len);
		O("\n");
	)
	O("\n");

	str = "Updated 10/21/2012: We have added all of the original Les Voyages Extrodanaire     French   cover art for each work, enjoy!Doma Publishing presents to you this Jules Verne Collection , which has been designed and formatted specifically for your Amazon Kindle . Unlike other e-book editions, the text and chapters are perfectly set up to match the layout and feel of a physical copy, rather than being haphazardly thrown together for a quick release. This edition also comes with a linked Table of Contents for both the list of included books and their respective chapters. Navigation couldn't be easier . Purchase this Jules Vernes Collection and treat yourself to the following list of works by this Father of Science Fiction : The Extraordinary Voyages Series(Les Voyages Extraordinaires) Five Weeks in a Balloon (1869)The Voyages and Adventures of Captain Hatteras (1874-75)A Journey to the Interior of the Earth (1871)From the Earth to the Moon (1867)In Search of the Castaways (1873)Twenty Thousand Leagues Under the Sea (1872)Around The Moon (1873) (Sequel to From the Earth to the Moon)The Fur Country (1873)Around the World in Eighty Days (1873)The Mysterious Island (1874)The Survivors of the Chancellor (1875)Michael Strogoff (1876)Off on a Comet (1877)The Child of the Cavern (1877)Dick Sand, A Captain at Fifteen (1878)Eight Hundred Leagues on the Amazon (1881)Godfrey Morgan (1883)The Lottery Ticket (1886)Robur the Conqueror (1887)Topsy-Turvy (1890)Claudius Bombarnac (1894)Facing the Flag (1897)An Antarctic Mystery (1898) Dr. Ox (A Series of Short Stories) Dr. Ox's ExperimentMaster ZachariusA Winer amid the IceA Drama in the AirFortieth Ascent of Mont Blanc Other Works The Pearl of Lima (1853)The Blockade Runners (1876)The Waif of the CynthiaIn the Year 2889 (1889)Master of the World (1904) (Sequel to Robur the Conqueror) Thank you for choosing Doma House Publishing. We look forward to creating many more affordable Kindle Classics for you to enjoy!";
	//str = "Updated 10/21/2012: We have added all of the original Les Voyages Extrodanaire     French   cover art for each work, enjoy!Doma Publishing presents to you this Jules Verne Collection , which has been designed and formatted specifically for your Amazon Kindle . Unlike other e-book editions, the text and chapters are perfectly set up to match the layout and feel of a physical copy, rather than being haphazardly thrown together for a quick release. This edition also comes with a linked Table of Contents for both the list of included books and their respective chapters. Navigation couldn't be easier . Purchase this Jules Vernes Collection and treat yourself to the following list of works by this Father of Science Fiction : The Extraordinary Voyages Series(Les Voyages Extraordinaires) Five Weeks in a Balloon (1869)The Voyages and Adventures of Captain Hatteras (1874-75)A Journey to the Interior of the Earth (1871)From the Earth to the Moon (1867)In Search of the Castaways (1873)Twenty Thousand Leagues Under the Sea (1872)Around The Moon (1873) (Sequel to From the Earth to the Moon)The Fur Country (1873)Around the World in Eighty Days (1873)The Mysterious Island (1874)The Survivors of the Chancellor (1875)Michael Strogoff (1876)Off on a Comet (1877)The Child of the Cavern (1877)Dick Sand, A Captain at Fifteen (1878)Eight Hundred Leagues on the Amazon (1881)Godfrey Morgan (1883)The Lottery Ticket (1886)Robur the Conqueror (1887)Topsy-Turvy (1890)Claudius Bombarnac (1894)Facing the Flag (1897)An Antarctic Mystery (1898) Dr. Ox (A Series of Short Stories) Dr. Ox's ExperimentMaster ZachariusA Winer amid the IceA Drama in the AirFortieth Ascent of Mont Blanc Other Works The Pearl of Lima (1853)The Blockade Runners (1876)The Waif of the CynthiaIn the Year 2889 (1889)Master of the World (1904) (Sequel to Robur the Conqueror) Thank you for choosing Doma House Publishing. We look forward to creating many more affordable Kindle Classi    ";

	O("%lu %s\n\n", scnt(str), str);
	str_wrap(str,80,
		O("(%4d %d) %.*s\n", line_start, line_len, line_len, str+line_start);
	)
}

#endif

//:~

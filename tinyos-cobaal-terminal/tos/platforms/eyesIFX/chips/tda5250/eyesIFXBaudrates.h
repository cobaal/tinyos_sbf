/* auto generated from tda5250BusResourceSettings.h using the following perl script: 

#!/usr/bin/perl -w
use strict;
my $state = "START";
my $br;
while(<>) {
    if(/UBR_1MHZ_/) {
	($br) = /UBR_1MHZ_(\d+)/;
	if($state eq "START") {
	    print '#if ';
	    $state = "MIDDLE";
	}
	else {
	    print '#elif ';
	}
	print 'TDA5250_UART_BAUDRATE == ', $br,"\n";
	print '#define TDA5250_UART_UBR UBR_1MHZ_', $br, "\n";
	print '#define TDA5250_UART_UMCTL UMCTL_1MHZ_', $br, "\n";
    }
}
print '#endif',"\n"

*/

#if TDA5250_UART_BAUDRATE == 35108
#define TDA5250_UART_UBR UBR_1MHZ_35108
#define TDA5250_UART_UMCTL UMCTL_1MHZ_35108
#elif TDA5250_UART_BAUDRATE == 10240
#define TDA5250_UART_UBR UBR_1MHZ_10240
#define TDA5250_UART_UMCTL UMCTL_1MHZ_10240
#elif TDA5250_UART_BAUDRATE == 10922
#define TDA5250_UART_UBR UBR_1MHZ_10922
#define TDA5250_UART_UMCTL UMCTL_1MHZ_10922
#elif TDA5250_UART_BAUDRATE == 11702
#define TDA5250_UART_UBR UBR_1MHZ_11702
#define TDA5250_UART_UMCTL UMCTL_1MHZ_11702
#elif TDA5250_UART_BAUDRATE == 12603
#define TDA5250_UART_UBR UBR_1MHZ_12603
#define TDA5250_UART_UMCTL UMCTL_1MHZ_12603
#elif TDA5250_UART_BAUDRATE == 13653
#define TDA5250_UART_UBR UBR_1MHZ_13653
#define TDA5250_UART_UMCTL UMCTL_1MHZ_13653
#elif TDA5250_UART_BAUDRATE == 14894
#define TDA5250_UART_UBR UBR_1MHZ_14894
#define TDA5250_UART_UMCTL UMCTL_1MHZ_14894
#elif TDA5250_UART_BAUDRATE == 16384
#define TDA5250_UART_UBR UBR_1MHZ_16384
#define TDA5250_UART_UMCTL UMCTL_1MHZ_16384
#elif TDA5250_UART_BAUDRATE == 18204
#define TDA5250_UART_UBR UBR_1MHZ_18204
#define TDA5250_UART_UMCTL UMCTL_1MHZ_18204
#elif TDA5250_UART_BAUDRATE == 20480
#define TDA5250_UART_UBR UBR_1MHZ_20480
#define TDA5250_UART_UMCTL UMCTL_1MHZ_20480
#elif TDA5250_UART_BAUDRATE == 23405
#define TDA5250_UART_UBR UBR_1MHZ_23405
#define TDA5250_UART_UMCTL UMCTL_1MHZ_23405
#elif TDA5250_UART_BAUDRATE == 27306
#define TDA5250_UART_UBR UBR_1MHZ_27306
#define TDA5250_UART_UMCTL UMCTL_1MHZ_27306
#elif TDA5250_UART_BAUDRATE == 32768
#define TDA5250_UART_UBR UBR_1MHZ_32768
#define TDA5250_UART_UMCTL UMCTL_1MHZ_32768
#elif TDA5250_UART_BAUDRATE == 40960
#define TDA5250_UART_UBR UBR_1MHZ_40960
#define TDA5250_UART_UMCTL UMCTL_1MHZ_40960
#endif

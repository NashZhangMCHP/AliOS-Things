NAME := digest_algorithm

$(NAME)_MBINS_TYPE := share
$(NAME)_SOURCES := digest_algorithm.c CheckSumUtils.c crc.c md5.c
GLOBAL_INCLUDES += .

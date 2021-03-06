/******************************************************************************
 * Copyright © 2014-2017 The SuperNET Developers.                             *
 *                                                                            *
 * See the AUTHORS, DEVELOPER-AGREEMENT and LICENSE files at                  *
 * the top-level directory of this distribution for the individual copyright  *
 * holder information and the developer policies on copyright and licensing.  *
 *                                                                            *
 * Unless otherwise agreed in a custom licensing agreement, no part of the    *
 * SuperNET software, including this file may be copied, modified, propagated *
 * or distributed except according to the terms contained in the LICENSE file *
 *                                                                            *
 * Removal or modification of this copyright notice is prohibited.            *
 *                                                                            *
 ******************************************************************************/

// included from basilisk.c

int32_t smartaddress_add(struct supernet_info *myinfo,bits256 privkey,char *BTCaddr,char *KMDaddr)
{
    struct smartaddress *ap;
    int32_t i;
    if ( myinfo->numsmartaddrs < sizeof(myinfo->smartaddrs)/sizeof(*myinfo->smartaddrs) )
    {
        for (i=0; i<myinfo->numsmartaddrs; i++)
            if ( bits256_cmp(myinfo->smartaddrs[i].privkey,privkey) == 0 )
                return(-1);
        ap = &myinfo->smartaddrs[myinfo->numsmartaddrs++];
        ap->privkey = privkey;
        bitcoin_pubkey33(myinfo->ctx,ap->pubkey33,privkey);
        calc_rmd160_sha256(ap->rmd160,ap->pubkey33,33);
        ap->pubkey = curve25519(privkey,curve25519_basepoint9());
        char coinaddr[64]; uint8_t addrtype,rmd160[20];
        bitcoin_address(coinaddr,0,ap->pubkey33,33);
        for (i=0; i<20; i++)
            printf("%02x",ap->rmd160[i]);
        bitcoin_addr2rmd160(&addrtype,rmd160,coinaddr);
        printf(", ");
        for (i=0; i<20; i++)
            printf("%02x",rmd160[i]);
        printf (" <- rmd160 for %d %s vs %s\n",myinfo->numsmartaddrs,coinaddr,BTCaddr);
        return(myinfo->numsmartaddrs);
    }
    printf("too many smartaddresses %d vs %d\n",myinfo->numsmartaddrs,(int32_t)(sizeof(myinfo->smartaddrs)/sizeof(*myinfo->smartaddrs)));
    return(-1);
}

int32_t smartaddress(struct supernet_info *myinfo,bits256 *privkeyp,char *coinaddr)
{
    int32_t i; uint8_t addrtype,rmd160[20];
    memset(privkeyp,0,sizeof(*privkeyp));
    bitcoin_addr2rmd160(&addrtype,rmd160,coinaddr);
    for (i=0; i<myinfo->numsmartaddrs; i++)
        if ( memcmp(myinfo->smartaddrs[i].rmd160,rmd160,20) == 0 )
        {
            *privkeyp = myinfo->smartaddrs[i].privkey;
            printf("MATCHED %s\n",coinaddr);
            return(i);
        }
    for (i=0; i<20; i++)
        printf("%02x",rmd160[i]);
    printf(" <- rmd160 smartaddress cant find (%s) of %d\n",coinaddr,myinfo->numsmartaddrs);
    return(-1);
}

int32_t smartaddress_pubkey(struct supernet_info *myinfo,bits256 *privkeyp,bits256 pubkey)
{
    int32_t i;
    memset(privkeyp,0,sizeof(*privkeyp));
    for (i=0; i<myinfo->numsmartaddrs; i++)
        if ( bits256_cmp(myinfo->smartaddrs[i].pubkey,pubkey) == 0 )
        {
            *privkeyp = myinfo->smartaddrs[i].privkey;
            return(i);
        }
    return(-1);
}

int32_t smartaddress_pubkey33(struct supernet_info *myinfo,bits256 *privkeyp,uint8_t *pubkey33)
{
    int32_t i;
    memset(privkeyp,0,sizeof(*privkeyp));
    for (i=0; i<myinfo->numsmartaddrs; i++)
        if ( memcmp(myinfo->smartaddrs[i].pubkey33,pubkey33,33) == 0 )
        {
            *privkeyp = myinfo->smartaddrs[i].privkey;
            return(i);
        }
    return(0);
}


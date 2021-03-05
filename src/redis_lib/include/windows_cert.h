//
// Created by nmuravev on 2/26/2021.
//

#ifndef REDISNATIVE_WINDOWS_CERT_H
#define REDISNATIVE_WINDOWS_CERT_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <wincrypt.h>

void add_windows_root_certs(boost::asio::ssl::context &ctx)
{
    HCERTSTORE hStore = CertOpenSystemStore(0, "ROOT");
    if (hStore == NULL) {
        return;
    }

    X509_STORE *store = X509_STORE_new();
    PCCERT_CONTEXT pContext = NULL;
    while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != NULL) {
        X509 *x509 = d2i_X509(NULL,
                              (const unsigned char **)&pContext->pbCertEncoded,
                              pContext->cbCertEncoded);
        if(x509 != NULL) {
            X509_STORE_add_cert(store, x509);
            X509_free(x509);
        }
    }

    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    SSL_CTX_set_cert_store(ctx.native_handle(), store);
}
#endif //REDISNATIVE_WINDOWS_CERT_H

#if !defined(__91C579E65F8D35578FBDAA7F55F__)
# define __91C579E65F8D35578FBDAA7F55F__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/io/io.h>

enum XPADF_SOCKET_ADDRESS_FAMILY {
  XPADF_SOCKET_ADDRESS_FAMILY_IPV4 = 0,
  XPADF_SOCKET_ADDRESS_FAMILY_IPV6 = 1,
  XPADF_SOCKET_ADDRESS_FAMILY_UNSPECIFIED
};

XPADF_DECLARE_ENUM(XPADF_SOCKET_ADDRESS_FAMILY);

XPADF_CALLBACK(XPADF_RESULT, PXPADFAcceptSocketCallback, (XPADF_IN     XPADF_HANDLE _hServerSocket,
                                                          XPADF_IN_OPT XPADF_PVOID  _pContext,
                                                          XPADF_IN     XPADF_HANDLE _hNewConnection,
                                                          XPADF_IN     XPADF_PVOID  _pAddress,
                                                          XPADF_IN     XPADF_SIZE   _nAddress));
XPADF_CALLBACK(XPADF_RESULT, PXPADFConnectSocketCallback, (XPADF_IN     XPADF_HANDLE _hConnectedSocket,
                                                           XPADF_IN_OPT XPADF_PVOID  _pContext));

#endif /* !__91C579E65F8D35578FBDAA7F55F__ */

#if !defined(__11626A01147B5775FCCE9D61D1D__)
# define __11626A01147B5775FCCE9D61D1D__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

# include <xpadf/common/objects.h>

# include <xpadf/io/socket.h>
# include <xpadf/io/iopoller.h>

# if defined(__cplusplus)
extern "C" {
# endif /* __cplusplus */

  XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateTCPServerSocket, (XPADF_OUT    PXPADF_HANDLE               _phTCPServerSocket,
                                                             XPADF_IN     XPADF_SOCKET_ADDRESS_FAMILY _eAddressHint,
                                                             XPADF_IN_OPT XPADF_PORT                  _nBindPort,
                                                             XPADF_IN_OPT CXPADF_STRING               _szBindAddress,
                                                             XPADF_IN     XPADF_HANDLE                _hIOPoller,
                                                             XPADF_IN_OPT XPADF_PVOID                 _pContext,
                                                             XPADF_IN     PXPADFAcceptSocketCallback  _pAcceptCallback,
                                                             XPADF_IN_OPT PXPADFIOCleanupCallback     _pCleanupCallback));
  XPADF_FUNCTION(XPADF_RESULT, xpadf_CreateTCPClientSocket, (XPADF_IN     XPADF_SOCKET_ADDRESS_FAMILY _eAddressHint,
                                                             XPADF_IN_OPT XPADF_PORT                  _nServerPort,
                                                             XPADF_IN     CXPADF_STRING               _szServerAddress,
                                                             XPADF_IN     XPADF_HANDLE                _hIOPoller,
                                                             XPADF_IN_OPT XPADF_PVOID                 _pContext,
                                                             XPADF_IN     PXPADFConnectSocketCallback _pConnectCallback,
                                                             XPADF_IN_OPT PXPADFIOCleanupCallback     _pCleanupCallback));

# if defined(__cplusplus)
}
# endif /* __cplusplus */

#endif /* !__11626A01147B5775FCCE9D61D1D__ */

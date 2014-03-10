#if !defined(__485A794F2A379947FFE97A93E19__)
# define __485A794F2A379947FFE97A93E19__

# include <xpadf/common/macros.h>
# include <xpadf/common/types.h>
# include <xpadf/common/status.h>

XPADF_DECLARE_STRUCT(PROTOCOL_HEADER);

struct PROTOCOL_HEADER {
  XPADF_SIZE m_nPacketSize;
};

#endif /* !__485A794F2A379947FFE97A93E19__ */

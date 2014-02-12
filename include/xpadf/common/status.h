#if !defined(__4245487EF026C4FE6C1ABBBB978__)
# define __4245487EF026C4FE6C1ABBBB978__

# define XPADF_OK 0x000000000

# define XPADF_DEBUG_MASK       0x010000000
# define XPADF_INFORMATION_MASK 0x020000000
# define XPADF_WARNING_MASK     0x040000000
# define XPADF_ERROR_MASK       0x080000000

# define XPADF_SUCCEEDED(__exp__) (XPADF_WARNING_MASK > (__exp__))
# define XPADF_FAILED(__exp__)    (XPADF_WARNING_MASK < (__exp__))

# define XPADF_CHECK_STATUS(__mask__, __exp__) (__mask__ & (__exp__))

# define XPADF_IS_DEBUG(__exp__)       XPADF_CHECK_STATUS(XPADF_DEBUG_MASK,       __exp__)
# define XPADF_IS_INFORMATION(__exp__) XPADF_CHECK_STATUS(XPADF_INFORMATION_MASK, __exp__)
# define XPADF_IS_WARNING(__exp__)     XPADF_CHECK_STATUS(XPADF_WARNING_MASK,     __exp__)
# define XPADF_IS_ERROR(__exp__)       XPADF_CHECK_STATUS(XPADF_ERROR_MASK,       __exp__)

# define XPADF_GLOBAL_MODULE 0x00000

# define XPADF_DEFINE_STATUS_CODE(__mask__, __module__, __code__) (__mask__ | ((__module__ & 0x00fff) << 16) | (__code__ & 0x0ffff))

#endif /* !__4245487EF026C4FE6C1ABBBB978__ */

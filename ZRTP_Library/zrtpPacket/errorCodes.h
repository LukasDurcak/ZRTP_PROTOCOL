#ifndef ERRORCODES_H
#define ERRORCODES_H

// Error codes are used in error message and in every message parser
enum zrtpErrorCode {
    N_ERROR                           = 0x00, // NO error
    MALFORMED_PACKET                  = 0x10, // CRC OK, but wrong structure
    CRITICAL_SOFTWARE_ERROR           = 0x20,
    UNSUPORTED_ZRTP_VERSION           = 0x30,
    HELLO_COMPONENTS_MISMATCH         = 0x40,
    HASH_TYPE_NOT_SUPPORTED           = 0x51,
    CIPHER_TYPE_NOT_SUPPORTED         = 0x52,
    PUBLIC_KEY_EXCHANGE_NOT_SUPPORTED = 0x53,
    SRTP_AUTH_TAG_NOT_SUPPORTED       = 0x54,
    SAS_RENDERING_NOT_SUPPORTED       = 0x55,
    DH_MODE_REQUIRED                  = 0x56, // No shared secret available
    DH_ERROR_BAD_PUBLIC_VALUE         = 0x61, // Bad pvi or pvr (== 1, 0, or p-1)
    DH_ERROR_HASHED_DATA              = 0x62, // Hvi != hashed data
    SAS_FROM_UNTRUSTED_MITM           = 0x63,
    AUTH_ERROR                        = 0x70, // Bad confirm packet MAC
    NONCE_REUSE                       = 0x80,
    EQUALS_ZID_IN_HELLO               = 0x90,
    SSRC_COLLISION                    = 0x91,
    SERVICE_UNAVAILABLE               = 0xA0,
    PROTOCOL_TIMEOUT_ERROR            = 0xB0,
    GOCLEAR_MESSAGE_ERROR             = 0x100 // GoClear message received, but not allowed
};

#endif // ERRORCODES_H

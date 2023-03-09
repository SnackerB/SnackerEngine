#pragma once

#include <cinttypes>

namespace SnackerEngine
{
    /// The MESSAGE_TYPE should be saved in the first entry of the SMP header
    enum class MESSAGE_TYPE : uint16_t
    {
        ECHO,                   /// Used for pings and alive-checks
        ERROR,                  /// Used for sending error messages
        ADVERTISEMENT,          /// Used to advertise a client to the SRP server
    };

    /// Options for the message type ECHO
    enum class MESSAGE_OPTION_ECHO : uint16_t
    {
        ECHO_REQUEST,           /// Requests an echo reply (optional data: time stamp)
        ECHO_REPLY,             /// answers to an echo request (optional data: time stamp)
        /* Data (optional) */
    };

    /// Options for the message type ERROR
    enum class MESSAGE_OPTION_ERROR : uint16_t
    {
        UNSPECIFIED,            /// Default (optional data: error message in UTF-8)
        TIMEOUT,                /// Some action has timed out (optional data: error message in UTF-8)
        NOT_FOUND,              /// Something was not found (optional data: error message in UTF-8)
        BAD_TYPE,               /// The wrong type was given (optional data: error message in UTF-8)
        BAD_OPTION,             /// A wrong option was specified (optional data: error message in UTF-8)
        TOO_MANY_CLIENTS,       /// Too many clients have already connected (optional data: error message in UTF-8)
        /* Data (optional) */
    };

    /// Options for the message type ADVERTISEMENT
    enum class MESSAGE_OPTION_ADVERTISEMENT : uint16_t
    {
        REQUEST,          /// advertises a client to the server
        OK,               /// server sends the client his new id (additional data: client id (uint16_t))
        DISCONNECT,       /// Client disconnecting from server, or server booting client
    };

    /// Snacker Message Protocol (SMP)
    struct SMP_Header
    {
        uint16_t type;
        uint16_t options;
        /* Data (optional) */
        SMP_Header()
            : type(-1), options(-1) {}
        SMP_Header(const MESSAGE_TYPE& messageType, const int16_t& options)
            : type(static_cast<int16_t>(messageType)), options(options) {};
        SMP_Header(const MESSAGE_TYPE& messageType, const MESSAGE_OPTION_ECHO& options)
            : SMP_Header(messageType, static_cast<int16_t>(options)) {};
        SMP_Header(const MESSAGE_TYPE& messageType, const MESSAGE_OPTION_ERROR& options)
            : SMP_Header(messageType, static_cast<int16_t>(options)) {};
        SMP_Header(const MESSAGE_TYPE& messageType, const MESSAGE_OPTION_ADVERTISEMENT& options)
            : SMP_Header(messageType, static_cast<int16_t>(options)) {};

        void turnToHostByteOrder();
        void turnToNetworkByteOrder();
    };
}

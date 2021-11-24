//
// Created by kiah on 2020/6/23.
//

#ifndef COREUTILS_STATUS_H
#define COREUTILS_STATUS_H

#include "../port/base_types.h"
#include <string>
#include <memory>

namespace cutils {

    class Status {
    public:
        Status() : mState(nullptr) {};

        Status(CUTILS_STATUS status);

        Status(CUTILS_STATUS status, std::string &&msg);

        Status(CUTILS_STATUS status, std::string &msg);

        inline Status(const Status &s) : mState((s.mState == NULL) ? NULL : new State(*s.mState)) {}

        ~Status() = default;

        std::string ToString() const;

        Status &operator=(const Status &s);

        Status &operator<<(const std::string &x);
        Status &operator<<(const int &x);

        // Returns true iff the status indicates success.
        bool ok() const;

        CUTILS_STATUS code() const;

        const std::string &message() const;

        bool operator==(const Status &x) const;

        bool operator!=(const Status &x) const;

        Status & update(CUTILS_STATUS code);

        inline void IgnoreError() const {
            // no-op
        }

        bool operator==(const CUTILS_STATUS &x) const;
        bool operator!=(const CUTILS_STATUS &x) const;

        static Status CombinedStatus(
                const std::string& general_comment,
                const std::vector<Status>& statuses);

    private:
        static const std::string &empty_string();

        struct State {
            CUTILS_STATUS code;
            std::string msg;
        };

        std::unique_ptr<State> mState;
    };

    inline Status OkStatus() { return Status(); }

    const char *ErrorMessage(CUTILS_STATUS code);
}


#endif //COREUTILS_STATUS_H

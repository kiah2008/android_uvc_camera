//
// Created by kiah on 2020/6/23.
//

#include <base/Status.h>

#include <functional>
#include <vector>
#include <utils/str_helper.h>
#include <port/clog.h>

using namespace cutils;


std::string Status::ToString() const {
    if (mState == nullptr) {
        return "OK";
    } else {
        const char *type = ErrorMessage(mState->code);
        std::string result(type);
        result += ": ";
        result += mState->msg;
        return result;
    }
}

Status::Status(CUTILS_STATUS status) {
    mState = std::unique_ptr<State>(new State);
    mState->code = status;
    mState->msg = std::string();
}

Status::Status(CUTILS_STATUS status, std::string &msg) {
    mState = std::unique_ptr<State>(new State);
    mState->code = status;
    mState->msg = std::string(msg);
}

Status::Status(CUTILS_STATUS status, std::string &&msg) {
    mState = std::unique_ptr<State>(new State);
    mState->code = status;
    mState->msg = std::string(msg);
}

const std::string &Status::empty_string() {
    static std::string *empty = new std::string;
    return *empty;
}

Status &Status::update(CUTILS_STATUS code) {
    if (mState == nullptr) {
        mState.reset(new State{.code = code});
    } else {
        mState->code = code;
    }
    return *this;
}

Status &Status::operator=(const Status &s) {
    if (mState != s.mState) {
        if (s.mState == NULL)
            mState = NULL;
        else {
            mState = std::unique_ptr<State>(new State(*(s.mState)));
        }
    }
    return *this;
}

Status &Status::operator<<(const std::string &x) {
    if (mState == nullptr) {
        mState.reset(new State{});
    }
    mState->msg += x;
    return *this;
}

CUTILS_STATUS Status::code() const {
    return ok() ? CUTILS_STATUS::CUTILS_STATUS_OK : mState->code;
}

bool Status::operator==(const Status &x) const {
    return (this->mState == x.mState) || (ToString() == x.ToString());
}

bool Status::operator!=(const Status &x) const { return !(*this == x); }

std::ostream &operator<<(std::ostream &os, const Status &x) {
    os << x.ToString();
    return os;
}

bool Status::ok() const {
    return (mState == NULL) || (mState->code == CUTILS_STATUS::CUTILS_STATUS_OK);
}

const std::string &Status::message() const {
    return ok() ? empty_string() : mState->msg;
}

bool Status::operator==(const CUTILS_STATUS &x) const {
    return this->code() == x;
}

bool Status::operator!=(const CUTILS_STATUS &x) const {
    return !(*this == x);
}

typedef std::function<void(const Status &)> StatusCallback;

Status Status::CombinedStatus(
        const std::string &general_comment,
        const std::vector<Status> &statuses) {
    // The final error code is StatusCode::kUnknown if not all
    // the error codes are the same.  Otherwise it is the same error code
    // as all of the (non-OK) statuses.  If statuses is empty or they are
    // all OK, then OkStatus() is returned.
    CUTILS_STATUS error_code = CUTILS_STATUS_OK;
    std::vector<std::string> errors;
    for (const Status &status : statuses) {
        if (!status.ok()) {
            errors.emplace_back(status.message());
            if (error_code == CUTILS_STATUS_OK) {
                error_code = status.code();
            } else if (error_code != status.code()) {
                error_code = CUTILS_STATUS_UNKNOWN;
            }
        }
    }
    return Status(
            error_code,
            StrCat(general_comment, "\n", StrJoin(errors, "\n")));
}

Status &Status::operator<<(const int &x) {
    if (mState == nullptr) {
        mState.reset(new State{});
    }
    mState->msg += std::to_string(x);
    return *this;
}


const char *cutils::ErrorMessage(CUTILS_STATUS code) {
    const char *info;
    switch (code) {
        case CUTILS_STATUS::CUTILS_STATUS_OK:
            info = "OK";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_CANCEL:
            info = "Cancelled";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_UNKNOWN:
            info = "Unknown";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_INVALID:
            info = "Invalid argument";
            break;
        case CUTILS_STATUS::CUTILS_DEADLINE_EXCEEDED:
            info = "Deadline exceeded";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_NOT_FOUND:
            info = "Not found";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_ALREADY_EXISTS:
            info = "Already exists";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_PERMISSION_DENNIED:
            info = "Permission denied";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_RESOURCE_EXHAUSTED:
            info = "Resource exhausted";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_FAIL_PRECONDITION:
            info = "Failed precondition";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_ABORT:
            info = "Aborted";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_OUT_OF_RANGE:
            info = "Out of range";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_UNIMPLEMENTATION:
            info = "Unimplemented";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_INTERNAL_ERROR:
            info = "Internal";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_UNAVAILABLE:
            info = "Unavailable";
            break;
        case CUTILS_STATUS::CUTILS_STATUS_DATALOSS:
            info = "Data loss";
            break;
        default:
            CLOGE("unknown type %d", code);
            info = "Unknown Type";
            assert(0);
            break;
    }
    return info;
}
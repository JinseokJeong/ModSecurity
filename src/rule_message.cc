/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 - 2023 Trustwave Holdings, Inc. (http://www.trustwave.com/)
 *
 * You may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * If any of the files related to licensing are missing or if you have any
 * other questions related to licensing please contact Trustwave Holdings, Inc.
 * directly using the email address security@modsecurity.org.
 *
 */

#include "modsecurity/rule_message.h"

#include "modsecurity/rules_set.h"
#include "modsecurity/modsecurity.h"
#include "modsecurity/transaction.h"
#include "src/utils/string.h"

namespace modsecurity {


std::string RuleMessage::_details(const RuleMessage *rm) {
    std::string msg;

    msg.append(" [file \"" + rm->m_rule.getFileName() + "\"]");
    msg.append(" [line \"" + std::to_string(rm->m_rule.getLineNumber()) + "\"]");
    msg.append(" [id \"" + std::to_string(rm->m_rule.m_ruleId) + "\"]");
    msg.append(" [rev \"" + utils::string::toHexIfNeeded(rm->m_rule.m_rev, true) + "\"]");
    msg.append(" [msg \"" + rm->m_message + "\"]");
    msg.append(" [data \"" + utils::string::toHexIfNeeded(utils::string::limitTo(200, rm->m_data), true) + "\"]");
    msg.append(" [severity \"" +
        std::to_string(rm->m_severity) + "\"]");
    msg.append(" [ver \"" + utils::string::toHexIfNeeded(rm->m_rule.m_ver, true) + "\"]");
    msg.append(" [maturity \"" + std::to_string(rm->m_rule.m_maturity) + "\"]");
    msg.append(" [accuracy \"" + std::to_string(rm->m_rule.m_accuracy) + "\"]");

    for (const auto &a : rm->m_tags) {
        msg.append(" [tag \"" + utils::string::toHexIfNeeded(a, true) + "\"]");
    }

    msg.append(" [hostname \"" + rm->m_transaction.m_requestHostName \
        + "\"]");
    msg.append(" [uri \"" + utils::string::limitTo(200, rm->m_transaction.m_uri_no_query_string_decoded) + "\"]");
    msg.append(" [unique_id \"" + rm->m_transaction.m_id + "\"]");
    msg.append(" [ref \"" + utils::string::limitTo(200, rm->m_reference) + "\"]");

    return msg;
}


std::string RuleMessage::_errorLogTail(const RuleMessage *rm) {
    std::string msg;

    msg.append("[hostname \"" + rm->m_transaction.m_serverIpAddress + "\"]");
    msg.append(" [uri \"" + utils::string::limitTo(200, rm->m_transaction.m_uri_no_query_string_decoded) + "\"]");
    msg.append(" [unique_id \"" + rm->m_transaction.m_id + "\"]");

    return msg;
}


std::string RuleMessage::log(const RuleMessage *rm, int props, int code) {
    std::string msg("");
    msg.reserve(2048);

    if (props & ClientLogMessageInfo) {
        msg.append("[client " + rm->m_transaction.m_clientIpAddress + "] ");
    }

    if (rm->m_isDisruptive) {
        msg.append("ModSecurity: Access denied with code ");
        if (code == -1) {
            msg.append("%d");
        } else {
            msg.append(std::to_string(code));
        }
        msg.append(" (phase ");
        msg.append(std::to_string(rm->getPhase()) + "). ");
    } else {
        msg.append("ModSecurity: Warning. ");
    }

    msg.append(rm->m_match);
    msg.append(_details(rm));

    if (props & ErrorLogTailLogMessageInfo) {
        msg.append(" " + _errorLogTail(rm));
    }

    return modsecurity::utils::string::toHexIfNeeded(msg);
}


}  // namespace modsecurity

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

// ─────────────────────────────────────────────
//  UTILITIES
// ─────────────────────────────────────────────

string toLowerCase(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

string trim(string s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
    return s;
}

// Returns true if any keyword in the list is found in text
bool containsAny(const string& text, const vector<string>& keywords) {
    for (const auto& kw : keywords)
        if (text.find(kw) != string::npos) return true;
    return false;
}

// ─────────────────────────────────────────────
//  SAFETY CHECK  (malicious / unsafe input)
// ─────────────────────────────────────────────

bool isMaliciousInput(const string& issue) {
    vector<string> malicious = {
        "drop table", "select *", "insert into", "delete from",  // SQL injection
        "<script", "javascript:", "onerror=",                     // XSS
        "../", "etc/passwd", "cmd.exe",                          // path traversal / shell
        "hack", "exploit", "bypass", "inject"
    };
    return containsAny(issue, malicious);
}

// ─────────────────────────────────────────────
//  PRODUCT AREA  (expanded keyword sets)
// ─────────────────────────────────────────────

string classify_product_area(const string& raw) {
    string issue = toLowerCase(raw);

    // Fraud & Security — check first (highest priority)
    if (containsAny(issue, {"fraud", "unauthorized", "identity", "hacked",
                             "scam", "phishing", "suspicious", "stolen"}))
        return "Fraud & Security";

    // Payments
    if (containsAny(issue, {"payment", "charged", "charge", "refund",
                             "transaction", "invoice", "receipt", "double charged",
                             "overcharged", "price", "cost", "fee"}))
        return "Payments";

    // Billing / Subscriptions
    if (containsAny(issue, {"subscription", "plan", "pause", "cancel",
                             "renew", "renewal", "downgrade", "upgrade", "tier"}))
        return "Billing";

    // Account Access
    if (containsAny(issue, {"login", "account", "access", "password",
                             "sign in", "signin", "locked", "2fa",
                             "two factor", "otp", "credentials", "email change"}))
        return "Account Access";

    // Assessments / Tests
    if (containsAny(issue, {"test", "assessment", "submission", "quiz",
                             "interview", "coding challenge", "exam", "attempt",
                             "proctoring", "time limit", "retake"}))
        return "Assessments";

    // API
    if (containsAny(issue, {"api", "request failing", "endpoint", "webhook",
                             "rate limit", "timeout", "sdk", "integration",
                             "rest", "graphql", "http", "401", "403", "500"}))
        return "API";

    // User Features
    if (containsAny(issue, {"certificate", "resume", "profile", "badge",
                             "dashboard", "leaderboard", "notification",
                             "settings", "download", "export", "report"}))
        return "User Features";

    return "General";
}

// ─────────────────────────────────────────────
//  REQUEST TYPE
// ─────────────────────────────────────────────

string classify_request(const string& raw) {
    string issue = toLowerCase(raw);

    if (issue.length() < 3)                                      return "invalid";

    if (containsAny(issue, {"error", "failed", "not working", "broken",
                             "crash", "bug", "issue", "problem", "doesn't work",
                             "can't", "cannot", "won't", "404", "500"}))
        return "bug";

    if (containsAny(issue, {"feature", "add", "improve", "suggest",
                             "wish", "would be nice", "request", "enhancement"}))
        return "feature_request";

    if (containsAny(issue, {"refund", "charged", "payment", "invoice"}))
        return "billing_query";

    if (containsAny(issue, {"subscription", "cancel", "pause", "plan"}))
        return "billing_query";

    if (containsAny(issue, {"fraud", "unauthorized", "hacked", "stolen"}))
        return "security_report";

    return "product_issue";
}

// ─────────────────────────────────────────────
//  ESCALATION DECISION
//  Escalate only when truly warranted — not just because category is General
// ─────────────────────────────────────────────

struct TriageResult {
    string action;
    string reason;
};

TriageResult decide_action(const string& raw, const string& product_area, const string& request_type) {
    string issue = toLowerCase(raw);

    // 1. Malicious input — escalate immediately
    if (isMaliciousInput(issue))
        return {"escalated", "Potentially malicious input detected → security escalation."};

    // 2. Vague / too short
    if (trim(raw).length() < 10)
        return {"escalated", "Input too vague or too short to classify reliably."};

    // 3. Fraud & Security always escalate
    if (product_area == "Fraud & Security")
        return {"escalated", "Fraud or security-related issue → escalated to security team."};

    // 4. High-risk payment signals
    if (containsAny(issue, {"charged twice", "double charged", "unauthorized charge",
                              "payment failed", "dispute", "chargeback"}))
        return {"escalated", "High-risk payment keyword detected → escalated for financial review."};

    // 5. General with very short text (can't confidently classify)
    if (product_area == "General" && trim(raw).length() < 30)
        return {"escalated", "Unable to classify — insufficient context."};

    // 6. General but text is long enough — reply with a generic helpful response
    //    instead of always escalating
    if (product_area == "General")
        return {"replied", "No specific category matched — general support response sent."};

    // 7. Everything else — reply
    return {"replied", "Issue classified successfully — standard response sent."};
}

// ─────────────────────────────────────────────
//  RESPONSE GENERATION
// ─────────────────────────────────────────────

string generate_response(const string& action, const string& product_area) {
    if (action == "escalated")
        return "Your issue has been escalated to our support team, who will reach out shortly.";

    if (product_area == "Payments")
        return "Please verify your payment details and transaction history. If the charge is incorrect, our billing team can process a correction — contact support with your transaction ID.";

    if (product_area == "Billing")
        return "You can manage your subscription (pause, cancel, or upgrade) from Account Settings → Billing. If you need further help, please contact support with your account email.";

    if (product_area == "Account Access")
        return "Try resetting your password via the 'Forgot Password' link. If you're still locked out or suspect unauthorized access, contact support immediately.";

    if (product_area == "API")
        return "Please check our API documentation and verify your API key, endpoint URL, and request format. Common issues include expired keys or malformed request bodies.";

    if (product_area == "Assessments")
        return "Ensure your browser meets the test requirements and your internet connection is stable. If a submission failed, contact support with your assessment ID and the time of the attempt.";

    if (product_area == "User Features")
        return "This feature can be accessed from your profile dashboard. If you're unable to find it or it's not working, please describe the exact steps and we'll investigate.";

    // General fallback
    return "Thank you for reaching out. Could you provide more details about the issue so we can assist you better?";
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────

int main() {
    ifstream file("support_issues.csv");
    ofstream out("output.csv");

    if (!file.is_open()) {
        cout << "Error: Could not open support_issues.csv\n";
        return 1;
    }

    out << "status,product_area,request_type,response,justification\n";

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        size_t comma_pos = line.find(',');
        if (comma_pos == string::npos) continue;

        string issue   = trim(line.substr(0, comma_pos));
        string company = trim(line.substr(comma_pos + 1));

        // Guard: empty row
        if (issue.empty()) {
            out << "escalated,General,invalid,"
                << "\"No input provided.\","
                << "\"Empty row — skipped.\"\n";
            continue;
        }

        string product_area = classify_product_area(issue);
        string request_type = classify_request(issue);
        TriageResult triage = decide_action(issue, product_area, request_type);
        string response     = generate_response(triage.action, product_area);

        // Escape any quotes in fields
        auto csvEscape = [](string s) {
            string result = "\"";
            for (char c : s) {
                if (c == '"') result += "\"\""; // double-quote escape
                else result += c;
            }
            result += "\"";
            return result;
        };

        out << triage.action    << ","
            << product_area     << ","
            << request_type     << ","
            << csvEscape(response)      << ","
            << csvEscape(triage.reason) << "\n";
    }

    file.close();
    out.close();

    cout << "Output written to output.csv\n";
    return 0;
}
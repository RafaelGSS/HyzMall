#pragma once
namespace http {
	enum http_code : unsigned int {
		http_response_custom_cannot_resolve = 1,
		http_response_continue = 100
		, http_response_switching_protocols = 101
		, http_response_ok = 200
		, http_response_created = 201
		, http_response_accepted = 202
		, http_response_nonauthoritative = 203
		, http_response_no_content = 204
		, http_response_reset_content = 205
		, http_response_partial_content = 206
		, http_response_multiple_choices = 300
		, http_response_moved_permanently = 301
		, http_response_found = 302
		, http_response_see_other = 303
		, http_response_not_modified = 304
		, http_response_useproxy = 305
		, http_response_temporary_redirect = 307
		, http_response_bad_request = 400
		, http_response_unauthorized = 401
		, http_response_payment_required = 402
		, http_response_forbidden = 403
		, http_response_not_found = 404
		, http_response_method_not_allowed = 405
		, http_response_not_acceptable = 406
		, http_response_proxy_authentication_required = 407
		, http_response_request_timeout = 408
		, http_response_conflict = 409
		, http_response_gone = 410
		, http_response_length_required = 411
		, http_response_precondition_failed = 412
		, http_response_requestentitytoolarge = 413
		, http_response_requesturitoolong = 414
		, http_response_unsupportedmediatype = 415
		, http_response_requested_range_not_satisfiable = 416
		, http_response_expectation_failed = 417
		, http_response_internal_server_error = 500
		, http_response_not_implemented = 501
		, http_response_bad_gateway = 502
		, http_response_service_unavailable = 503
		, http_response_gateway_timeout = 504
		, http_response_version_not_supported = 505
		, http_response_variant_also_negociates = 506
		, http_response_insufficient_storage = 507
		, http_response_loop_detected = 508
		, http_response_not_extended = 510
		, http_response_network_auth_required = 511
		, http_response_total
		, http_response_none = http_response_total
	};
}
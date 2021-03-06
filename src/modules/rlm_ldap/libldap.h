/**
 * $Id$
 * @file libldap.h
 * @brief Common utility functions for interacting with LDAP directories
 *
 * @note Do not rename to ldap.h.  This causes configure checks to break
 *	in stupid ways, where the configure script will use the local ldap.h
 *	file, instead of the one from libldap.
 *
 * @author Arran Cudbard-Bell <a.cudbardb@freeradius.org>
 *
 * @copyright 2017 The FreeRADIUS Server Project.
 * @copyright 2017 Arran Cudbard-Bell <a.cudbardb@freeradius.org>
 */
#ifndef LIBFREERADIUS_LDAP_H
#define	LIBFREERADIUS_LDAP_H

#include <freeradius-devel/radiusd.h>
#include <lber.h>
#include <ldap.h>
#include "config.h"

extern LDAP *ldap_global_handle;

/*
 *	Framework on OSX doesn't export the symbols but leaves
 *	the macro defined *sigh*.
 */
#ifndef HAVE_LDAP_CREATE_SESSION_TRACKING_CONTROL
#  undef LDAP_CONTROL_X_SESSION_TRACKING
#endif

/*
 *	There's a typo in libldap's ldap.h which was fixed by
 *	Howard Chu in 19aeb1cd. This typo had the function defined
 *	as ldap_create_session_tracking_control but declared as
 *	ldap_create_session_tracking.
 *
 *	We fix this, by adding the correct declaration here.
 */
#ifdef LDAP_CONTROL_X_SESSION_TRACKING
#  if !defined(HAVE_DECL_LDAP_CREATE_SESSION_TRACKING_CONTROL) || (HAVE_DECL_LDAP_CREATE_SESSION_TRACKING_CONTROL == 0)
LDAP_F( int )
ldap_create_session_tracking_control LDAP_P((
        LDAP            *ld,
        char            *sessionSourceIp,
        char            *sessionSourceName,
        char            *formatOID,
        struct berval   *sessionTrackingIdentifier,
        LDAPControl     **ctrlp ));
#  endif
#endif

/*
 *	Ensure the have the ldap_create_sort_keylist()
 *	function too, else we can't use ldap_create_sort_control()
 */
#if !defined(HAVE_LDAP_CREATE_SORT_KEYLIST) || !defined(HAVE_LDAP_FREE_SORT_KEYLIST)
#  undef HAVE_LDAP_CREATE_SORT_CONTROL
#endif

/*
 *	Because the LTB people define LDAP_VENDOR_VERSION_PATCH
 *	as X, which precludes its use in printf statements *sigh*
 *
 *	Identifiers that are not macros, all evaluate to 0,
 *	which is why this works.
 */
#if !defined(LDAP_VENDOR_VERSION_PATCH) || LDAP_VENDOR_VERSION_PATCH == 0
#  undef LDAP_VENDOR_VERSION_PATCH
#  define LDAP_VENDOR_VERSION_PATCH 0
#endif

/*
 *      For compatibility with other LDAP libraries
 */
#if !defined(LDAP_SCOPE_BASE) && defined(LDAP_SCOPE_BASEOBJECT)
#  define LDAP_SCOPE_BASE LDAP_SCOPE_BASEOBJECT
#endif

#if !defined(LDAP_SCOPE_ONE) && defined(LDAP_SCOPE_ONELEVEL)
#  define LDAP_SCOPE_ONE LDAP_SCOPE_ONELEVEL
#endif

#if !defined(LDAP_SCOPE_SUB) && defined(LDAP_SCOPE_SUBTREE)
#  define LDAP_SCOPE_SUB LDAP_SCOPE_SUBTREE
#endif

#if !defined(LDAP_OPT_RESULT_CODE) && defined(LDAP_OPT_ERROR_NUMBER)
#  define LDAP_OPT_RESULT_CODE LDAP_OPT_ERROR_NUMBER
#endif

#ifndef LDAP_CONST
#  define LDAP_CONST
#endif

#if defined(HAVE_LDAP_URL_PARSE) && defined(HAVE_LDAP_IS_LDAP_URL) && defined(HAVE_LDAP_URL_DESC2STR)
#  define LDAP_CAN_PARSE_URLS
#endif

#define LDAP_MAX_CONTROLS		10		//!< Maximum number of client/server controls.
							//!< Used to allocate static arrays of control pointers.
#define LDAP_MAX_ATTRMAP		128		//!< Maximum number of mappings between LDAP and
							//!< FreeRADIUS attributes.
#define LDAP_MAP_RESERVED		4		//!< Number of additional items to allocate in expanded
							//!< attribute name arrays. Currently for enable attribute,
							//!< group membership attribute, valuepair attribute,
							//!< and profile attribute.

#define LDAP_MAX_CACHEABLE		64		//!< Maximum number of groups we retrieve from the server for
							//!< a given user. If more than this number are retrieve the
							//!< module returns invalid.

#define LDAP_MAX_GROUP_NAME_LEN		128		//!< Maximum name of a group name.
#define LDAP_MAX_ATTR_STR_LEN		256		//!< Maximum length of an xlat expanded LDAP attribute.
#define LDAP_MAX_FILTER_STR_LEN		1024		//!< Maximum length of an xlat expanded filter.
#define LDAP_MAX_DN_STR_LEN		1024		//!< Maximum length of an xlat expanded DN.

#define LDAP_VIRTUAL_DN_ATTR		"dn"		//!< 'Virtual' attribute which maps to the DN of the object.


typedef enum {
	LDAP_EXT_UNSUPPORTED,				//!< Unsupported extension.
	LDAP_EXT_BINDNAME,				//!< Specifies the user DN or name for an LDAP bind.
	LDAP_EXT_BINDPW,				//!< Specifies the password for an LDAP bind.
} ldap_supported_extension_t;

typedef struct ldap_sasl {
	char const	*mech;				//!< SASL mech(s) to try.
	char const	*proxy;				//!< Identity to proxy.
	char const	*realm;				//!< Kerberos realm.
} ldap_sasl;

typedef struct fr_ldap_control {
	LDAPControl 	*control;			//!< LDAP control.
	bool		freeit;				//!< Whether the control should be freed after
							//!< we've finished using it.
} fr_ldap_control_t;

typedef enum {
	LDAP_DIRECTORY_UNKNOWN = 0,			//!< We can't determine the directory server.

	LDAP_DIRECTORY_ACTIVE_DIRECTORY,		//!< Directory server is Active Directory.
	LDAP_DIRECTORY_EDIRECTORY,			//!< Directory server is eDir.
	LDAP_DIRECTORY_IBM,				//!< Directory server is IBM.
	LDAP_DIRECTORY_NETSCAPE,			//!< Directory server is Netscape.
	LDAP_DIRECTORY_OPENLDAP,			//!< Directory server is OpenLDAP.
	LDAP_DIRECTORY_ORACLE_INTERNET_DIRECTORY,	//!< Directory server is Oracle Internet Directory.
	LDAP_DIRECTORY_ORACLE_UNIFIED_DIRECTORY,	//!< Directory server is Oracle Unified Directory.
	LDAP_DIRECTORY_ORACLE_VIRTUAL_DIRECTORY,	//!< Directory server is Oracle Virtual Directory.
	LDAP_DIRECTORY_SUN_ONE_DIRECTORY,		//!< Directory server is Sun One Directory.
	LDAP_DIRECTORY_SIEMENS_AG,			//!< Directory server is Siemens AG.
	LDAP_DIRECTORY_UNBOUND_ID			//!< Directory server is Unbound ID
} ldap_directory_type_t;

typedef struct ldap_directory {
	char const		*vendor_str;		//!< As returned from the vendorName attribute in the
							//!< rootDSE.
	char const		*version_str;		//!< As returned from the vendorVersion attribute in the
							//!< rootDSE.
	ldap_directory_type_t	type;			//!< Cannonical server implementation.

	bool			cleartext_password;	//!< Whether the server will return the user's plaintext
							//!< password.
} ldap_directory_t;

/** Connection configuration
 *
 * Must not be passed into functions except via the connection handle
 * this avoids problems with not using the connection pool configuration.
 */
typedef struct {
	char const	*name;				//!< Name of the module that created this connection.

	char		*server;			//!< Initial server to bind to.
	char const	**server_str;			//!< Server set in the config.

	uint16_t	port;				//!< Port to use when binding to the server.

	char const	*admin_identity;		//!< Identity we bind as when we need to query the LDAP
							//!< directory.
	char const	*admin_password;		//!< Password used in administrative bind.

	ldap_sasl	admin_sasl;			//!< SASL parameters used when binding as the admin.

	int		dereference;			//!< libldap value specifying dereferencing behaviour.
	char const	*dereference_str;		//!< When to dereference (never, searching, finding, always)

	bool		chase_referrals;		//!< If the LDAP server returns a referral to another server
							//!< or point in the tree, follow it, establishing new
							//!< connections and binding where necessary.
	bool		chase_referrals_unset;		//!< If true, use the OpenLDAP defaults for chase_referrals.

	bool		use_referral_credentials;	//!< If true use credentials from the referral URL.

	bool		rebind;				//!< Controls whether we set an ldad_rebind_proc function
							//!< and so determines if we can bind to other servers whilst
							//!< chasing referrals. If this is false, we will still chase
							//!< referrals on the same server, but won't bind to other
							//!< servers.

	/*
	 *	TLS items.
	 */
	int		tls_mode;

	bool		start_tls;			//!< Send the Start TLS message to the LDAP directory
							//!< to start encrypted communications using the standard
							//!< LDAP port.

	char const	*tls_ca_file;			//!< Sets the full path to a CA certificate (used to validate
							//!< the certificate the server presents).

	char const	*tls_ca_path;			//!< Sets the path to a directory containing CA certificates.

	char const	*tls_certificate_file;		//!< Sets the path to the public certificate file we present
							//!< to the servers.

	char const	*tls_private_key_file;		//!< Sets the path to the private key for our public
							//!< certificate.

	char const	*tls_require_cert_str;		//!< Sets requirements for validating the certificate the
							//!< server presents.

	int		tls_require_cert;		//!< OpenLDAP constant representing the require cert string.


	/*
	 *	For keep-alives.
	 */
#ifdef LDAP_OPT_X_KEEPALIVE_IDLE
	uint32_t	keepalive_idle;			//!< Number of seconds a connections needs to remain idle
							//!< before TCP starts sending keepalive probes.
#endif
#ifdef LDAP_OPT_X_KEEPALIVE_PROBES
	uint32_t	keepalive_probes;		//!< Number of missed timeouts before the connection is
							//!< dropped.
#endif
#ifdef LDAP_OPT_X_KEEPALIVE_INTERVAL
	uint32_t	keepalive_interval;		//!< Interval between keepalive probes.
#endif

	/*
	 *	Search timelimits
	 */
	uint32_t	srv_timelimit;			//!< How long the server should spent on a single request
							//!< (also bounded by value on the server).

	struct timeval	net_timeout;			//!< How long we wait in blocking network calls.
							//!< We set this in the LDAP API, even though with
							//!< async calls, we control this using our event loop.
							//!< This is just in case there are blocking calls which
							//!< happen internally which we can't work around.

	struct timeval	res_timeout;			//!< How long we wait for results.
} ldap_handle_config_t;

/** Tracks the state of a libldap connection handle
 *
 */
typedef struct ldap_handle {
	LDAP		*handle;			//!< libldap handle.
	bool		rebound;			//!< Whether the connection has been rebound to something
							//!< other than the admin user.
	bool		referred;			//!< Whether the connection is now established a server
							//!< other than the configured one.

	fr_ldap_control_t serverctrls[LDAP_MAX_CONTROLS + 1];	//!< Server controls to use for all operations with
								//!< this handle.
	fr_ldap_control_t clientctrls[LDAP_MAX_CONTROLS + 1];	//!< Client controls to use for all operations with
								//!< this handle.
	int		serverctrls_cnt;		//!< Number of server controls associated with the handle.
	int		clientctrls_cnt;		//!< Number of client controls associated with the handle.

	ldap_directory_t *directory;			//!< The type of directory we're connected to.

	ldap_handle_config_t const *config;		//!< rlm_ldap connection configuration.
} ldap_handle_t;

/** Contains a collection of values
 *
 */
typedef struct fr_ldap_result {
	struct berval	**values;			//!< libldap struct containing bv_val (char *)
							//!< and length bv_len.
	int		count;				//!< Number of values.
} fr_ldap_result_t;

/** Codes returned by fr_ldap internal functions
 *
 */
typedef enum {
	LDAP_PROC_CONTINUE = 1,				//!< Operation is in progress.
	LDAP_PROC_SUCCESS = 0,				//!< Operation was successfull.

	LDAP_PROC_ERROR	= -1,				//!< Unrecoverable library/server error.

	LDAP_PROC_BAD_CONN	= -2,				//!< Transitory error, caller should retry the operation
							//!< with a new connection.

	LDAP_PROC_NOT_PERMITTED = -3,			//!< Operation was not permitted, either current user was
							//!< locked out in the case of binds, or has insufficient
							//!< access.

	LDAP_PROC_REJECT = -4,				//!< Bind failed, user was rejected.

	LDAP_PROC_BAD_DN = -5,				//!< Specified an invalid object in a bind or search DN.

	LDAP_PROC_NO_RESULT = -6,			//!< Got no results.

	LDAP_PROC_TIMEOUT = -7				//!< Operation timed out.
} ldap_rcode_t;


extern FR_NAME_NUMBER const ldap_supported_extensions[];

extern FR_NAME_NUMBER const ldap_scope[];
extern FR_NAME_NUMBER const ldap_tls_require_cert[];

/*
 *	ldap.c - Wrappers arounds OpenLDAP functions.
 */
void		fr_ldap_timeout_debug(REQUEST *request, ldap_handle_t const *conn,
				      struct timeval const *timeout, char const *prefix);

size_t		fr_ldap_escape_func(UNUSED REQUEST *request, char *out, size_t outlen, char const *in, UNUSED void *arg);

size_t		fr_ldap_unescape_func(UNUSED REQUEST *request, char *out, size_t outlen, char const *in, UNUSED void *arg);

ssize_t		fr_ldap_xlat_filter(REQUEST *request, char const **sub, size_t sublen, char *out, size_t outlen);

ldap_rcode_t	fr_ldap_bind(REQUEST *request,
			     ldap_handle_t **pconn,
			     char const *dn, char const *password,
#ifdef WITH_SASL
			     ldap_sasl const *sasl,
#else
			     NDEBUG_UNUSED ldap_sasl const *sasl,
#endif
			     struct timeval const *timeout,
			     LDAPControl **serverctrls, LDAPControl **clientctrls);

char const	*fr_ldap_error_str(ldap_handle_t const *conn);

ldap_rcode_t	fr_ldap_search(LDAPMessage **result, REQUEST *request,
			       ldap_handle_t **pconn,
			       char const *dn, int scope, char const *filter, char const * const * attrs,
			       LDAPControl **serverctrls, LDAPControl **clientctrls);

ldap_rcode_t	fr_ldap_modify(REQUEST *request, ldap_handle_t **pconn,
			       char const *dn, LDAPMod *mods[],
			       LDAPControl **serverctrls, LDAPControl **clientctrls);


ldap_rcode_t	fr_ldap_result(ldap_handle_t const *conn, int msgid, char const *dn,
			       struct timeval const *timeout,
			       LDAPMessage **result, char const **error, char **extra);

ldap_handle_t	*fr_ldap_conn_alloc(TALLOC_CTX *ctx, ldap_handle_config_t const *handle_config);

int		fr_ldap_conn_timeout_set(ldap_handle_t const *conn, struct timeval const *timeout);

int		fr_ldap_conn_timeout_reset(ldap_handle_t const *conn);

int		fr_ldap_global_config(int debug_level, char const *tls_random_file);

int		fr_ldap_global_init(void);

void		fr_ldap_global_free(void);

/*
 *	control.c - Connection based client/server controls
 */
void		fr_ldap_control_merge(LDAPControl *serverctrls_out[],
				      LDAPControl *clientctrls_out[],
				      size_t serverctrls_len,
				      size_t clientctrls_len,
				      ldap_handle_t *conn,
				      LDAPControl *serverctrls_in[],
				      LDAPControl *clientctrls_in[]);

int		fr_ldap_control_add_server(ldap_handle_t *conn, LDAPControl *ctrl, bool freeit);

int		fr_ldap_control_add_client(ldap_handle_t *conn, LDAPControl *ctrl, bool freeit);

void		fr_ldap_control_clear(ldap_handle_t *conn);

int		fr_ldap_control_add_session_tracking(ldap_handle_t *conn, REQUEST *request);

/*
 *	directory.c - Get directory capabilities from the remote server
 */
int		fr_ldap_directory_alloc(TALLOC_CTX *ctx, ldap_directory_t **out, ldap_handle_t **pconn);


/*
 *	sasl.c - SASL bind functions
 */
ldap_rcode_t	fr_ldap_sasl_interactive(REQUEST *request,
				      	 ldap_handle_t *pconn, char const *dn,
				      	 char const *password, ldap_sasl const *sasl,
				      	 LDAPControl **serverctrls, LDAPControl **clientctrls,
				      	 struct timeval const *timeout,
				      	 char const **error, char **error_extra);

/*
 *	uti.c - Utility functions
 */
size_t		fr_ldap_common_dn(char const *full, char const *part);

bool		fr_ldap_util_is_dn(char const *in, size_t inlen);

size_t		fr_ldap_util_normalise_dn(char *out, char const *in);

char		*fr_ldap_berval_to_string(TALLOC_CTX *ctx, struct berval const *in);

int		fr_ldap_parse_url_extensions(LDAPControl **sss, REQUEST *request,
					     ldap_handle_t *conn, char **extensions);


#endif

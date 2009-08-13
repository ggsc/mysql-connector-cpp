/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <sqlstring.h>
#include <exception.h>

#include "../mysql_util.h"
#include "../mysql_connection_options.h"

#include "mysql_client_api.h"
#include "mysql_resultset_proxy.h"
#include "mysql_statement_proxy.h"

#include "mysql_native_connection_wrapper.h"


namespace sql
{
namespace mysql
{
namespace NativeAPI
{


/* {{{ createStMysqlWrapper() */
NativeConnectionWrapper *
createNativeConnectionWrapper(const SQLString & clientFileName)
{
	return new MySQL_NativeConnectionWrapper(clientFileName);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::MySQL_NativeConnectionWrapper() */
MySQL_NativeConnectionWrapper::MySQL_NativeConnectionWrapper(const ::sql::SQLString & clientFileName)
	: api(::sql::mysql::NativeAPI::getCApiHandle(clientFileName)), mysql(api->init(NULL))
{
	if (mysql == NULL) {
		throw sql::SQLException("Insufficient memory: cannot create MySQL handle using mysql_init()");
	}
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::~MySQL_NativeConnectionWrapper() */
MySQL_NativeConnectionWrapper::~MySQL_NativeConnectionWrapper()
{
	api->close(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::affected_rows() */
uint64_t
MySQL_NativeConnectionWrapper::affected_rows()
{
	return api->affected_rows(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::autocommit() */
bool
MySQL_NativeConnectionWrapper::autocommit(bool mode)
{
	return (api->autocommit(mysql, mode) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::connect() */
bool
MySQL_NativeConnectionWrapper::connect(const ::sql::SQLString & host,
									const ::sql::SQLString & user,
									const ::sql::SQLString & passwd,
									const ::sql::SQLString & db,
									unsigned int			 port,
									const ::sql::SQLString & socket_or_pipe,
									unsigned long			client_flag)
{
	return (NULL != api->real_connect(mysql, host.c_str(), user.c_str(), passwd.c_str(),
										db.length() > 0 ? db.c_str() : NULL, port,
										socket_or_pipe.length() > 0 ? socket_or_pipe.c_str() : NULL, client_flag));
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::commit() */
bool 
MySQL_NativeConnectionWrapper::commit()
{
	return (api->commit(mysql) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::debug() */
void
MySQL_NativeConnectionWrapper::debug(const SQLString & debug)
{
	api->debug(debug.c_str());
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::errNo() */
unsigned int
MySQL_NativeConnectionWrapper::errNo()
{
	return api->mysql_errno(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::error() */
SQLString
MySQL_NativeConnectionWrapper::error()
{
	return api->error(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::field_count() */
unsigned int
MySQL_NativeConnectionWrapper::field_count()
{
	return api->field_count(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_client_version() */
unsigned long
MySQL_NativeConnectionWrapper::get_client_version()
{
	return api->get_client_version();
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_server_info() */
const SQLString &
MySQL_NativeConnectionWrapper::get_server_info()
{
	serverInfo= api->get_server_info(mysql);
	return serverInfo;
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_server_version() */
unsigned long
MySQL_NativeConnectionWrapper::get_server_version()
{
	return api->get_server_version(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::more_results() */
bool
MySQL_NativeConnectionWrapper::more_results()
{
	return (api->more_results(mysql) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::next_result() */
int
MySQL_NativeConnectionWrapper::next_result()
{
	return api->next_result(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::options() */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option, const void * value)
{
	return api->options(mysql, static_cast< mysql_option >(option), value);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::query() */
int
MySQL_NativeConnectionWrapper::query(const SQLString & stmt_str)
{
	return api->real_query(mysql, stmt_str.c_str(), stmt_str.length());
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::rollback() */
bool
MySQL_NativeConnectionWrapper::rollback()
{
	return (api->rollback(mysql) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::sqlstate() */
SQLString
MySQL_NativeConnectionWrapper::sqlstate()
{
	return api->sqlstate(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::ssl_set() */
bool
MySQL_NativeConnectionWrapper::ssl_set(const SQLString & key,
								const SQLString & cert,
								const SQLString & ca,
								const SQLString & capath,
								const SQLString & cipher)
{
	return ('\0' != api->ssl_set(mysql, key.c_str(), cert.c_str(), ca.c_str() , capath.c_str(), cipher.c_str()));
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::store_result() */
Resultset_Proxy *
MySQL_NativeConnectionWrapper::store_result()
{
	::st_mysql_res * raw= api->store_result(mysql);

	if (raw == NULL) {
		/*CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
			this->errNo(), this->sqlstate(), this->error());*/
		return NULL;
	}

	return new MySQL_Resultset_Proxy(raw, api);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::use_result() */
Resultset_Proxy *
MySQL_NativeConnectionWrapper::use_result()
{
	::st_mysql_res * raw= api->use_result(mysql);

	if (raw == NULL) {
		/*CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
							this->errNo(), this->sqlstate(), this->error());*/
		return NULL;
	}

	return new MySQL_Resultset_Proxy(raw, api);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::stmt_init() */
Statement_Proxy &
MySQL_NativeConnectionWrapper::stmt_init()
{
	::st_mysql_stmt * raw= api->stmt_init(mysql);

	if (raw == NULL) {
		/*CPP_ERR_FMT("No statement : %d:(%s) %s", e->errNo(), proxy->sqlstate(), proxy->error());*/
		::sql::mysql::util::throwSQLException(*this);
	}

	return *(new MySQL_Statement_Proxy(raw, api, this));
}
/* }}} */

} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

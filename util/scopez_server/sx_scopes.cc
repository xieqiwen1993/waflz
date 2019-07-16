//: ----------------------------------------------------------------------------
//: Copyright (C) 2019 Verizon.  All Rights Reserved.
//: All Rights Reserved
//:
//: \file:    sx_scopes.cc
//: \details: TODO
//: \author:  Reed P. Morrison
//: \date:    09/30/2019
//:
//:   Licensed under the Apache License, Version 2.0 (the "License");
//:   you may not use this file except in compliance with the License.
//:   You may obtain a copy of the License at
//:
//:       http://www.apache.org/licenses/LICENSE-2.0
//:
//:   Unless required by applicable law or agreed to in writing, software
//:   distributed under the License is distributed on an "AS IS" BASIS,
//:   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//:   See the License for the specific language governing permissions and
//:   limitations under the License.
//:
//: ----------------------------------------------------------------------------
//: ----------------------------------------------------------------------------
//: includes
//: ----------------------------------------------------------------------------
#include "sx_scopes.h"
#include "waflz/engine.h"
#include "waflz/rqst_ctx.h"
#include "is2/support/trace.h"
#include "is2/support/nbq.h"
#include "is2/support/ndebug.h"
#include "is2/srvr/api_resp.h"
#include "is2/srvr/srvr.h"
#include "jspb/jspb.h"
#include "support/file_util.h"
#include "support/string_util.h"
#include "event.pb.h"
#include "config.pb.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
//: ----------------------------------------------------------------------------
//: constants
//: ----------------------------------------------------------------------------
#ifndef STATUS_OK
  #define STATUS_OK 0
#endif
#ifndef STATUS_ERROR
  #define STATUS_ERROR -1
#endif
#define _SCOPEZ_SERVER_SCOPES_ID "waf-scopes-id"
namespace ns_scopez_server {
#if 0
//: ----------------------------------------------------------------------------
//: type
//: ----------------------------------------------------------------------------
typedef struct _waf_instance_update {
        char *m_buf;
        uint32_t m_buf_len;
        ns_waflz::instances *m_instances;
} waf_instance_update_t;
//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
static void *t_load_instance(void *a_context)
{
        waf_instance_update_t *l_i = reinterpret_cast<waf_instance_update_t *>(a_context);
        if(!l_i)
        {
                return NULL;
        }
        int32_t l_s;
        ns_waflz::instance *l_instance = NULL;
        l_s = l_i->m_instances->load_config(&l_instance, l_i->m_buf, l_i->m_buf_len, true);
        if(l_s != WAFLZ_STATUS_OK)
        {
                TRC_ERROR("performing m_profile->load_config\n");
                if(l_i->m_buf) { free(l_i->m_buf); l_i->m_buf = NULL;}
                return NULL;
        }
        if(l_i->m_buf) { free(l_i->m_buf); l_i->m_buf = NULL;}
        delete l_i;
        return NULL;
}
#endif
//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
ns_is2::h_resp_t update_scopes_h::do_post(ns_is2::session &a_session,
                                             ns_is2::rqst &a_rqst,
                                             const ns_is2::url_pmap_t &a_url_pmap)
{
#if 0
        uint64_t l_buf_len = a_rqst.get_body_len();
        ns_is2::nbq *l_q = a_rqst.get_body_q();
        // copy to buffer
        char *l_buf;
        l_buf = (char *)malloc(l_buf_len);
        l_q->read(l_buf, l_buf_len);
        m_instances->set_locking(true);
        if(!m_bg_load)
        {
                // TODO get status
                //ns_is2::mem_display((const uint8_t *)l_buf, (uint32_t)l_buf_len);
                int32_t l_s;
                ns_waflz::instance *l_instance = NULL;
                l_s = m_instances->load_config(&l_instance, l_buf, l_buf_len, true);
                if(l_s != WAFLZ_STATUS_OK)
                {
                        TRC_ERROR("performing m_profile->load_config\n");
                        if(l_buf) { free(l_buf); l_buf = NULL;}
                        return ns_is2::H_RESP_SERVER_ERROR;
                }
                if(l_buf) { free(l_buf); l_buf = NULL;}
        }
        else
        {
                waf_instance_update_t *l_instance_update = NULL;
                l_instance_update = new waf_instance_update_t();
                l_instance_update->m_buf = l_buf;
                l_instance_update->m_buf_len = l_buf_len;
                l_instance_update->m_instances = m_instances;
                pthread_t l_t_thread;
                int32_t l_pthread_error = 0;
                l_pthread_error = pthread_create(&l_t_thread,
                                                 NULL,
                                                 t_load_instance,
                                                 l_instance_update);
                if (l_pthread_error != 0)
                {
                        return ns_is2::H_RESP_SERVER_ERROR;
                }
        }
#endif
        std::string l_resp_str = "{\"status\": \"success\"}";
        ns_is2::api_resp &l_api_resp = ns_is2::create_api_resp(a_session);
        l_api_resp.add_std_headers(ns_is2::HTTP_STATUS_OK,
                                   "application/json",
                                   l_resp_str.length(),
                                   a_rqst.m_supports_keep_alives,
                                   a_session.get_server_name());
        l_api_resp.set_body_data(l_resp_str.c_str(), l_resp_str.length());
        l_api_resp.set_status(ns_is2::HTTP_STATUS_OK);
        ns_is2::queue_api_resp(a_session, l_api_resp);
        return ns_is2::H_RESP_DONE;
}
//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
sx_scopes::sx_scopes(void):
        m_bg_load(false),
        m_engine(NULL),
        m_update_scopes_h(NULL),
        m_scopes_configs(NULL),
        m_config_path()
{

}
//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
sx_scopes::~sx_scopes(void)
{
        if(m_engine) { delete m_engine; m_engine = NULL; }
        if(m_update_scopes_h) { delete m_update_scopes_h; m_update_scopes_h = NULL; }
        if(m_scopes_configs) { delete m_scopes_configs; m_scopes_configs = NULL; }
}
//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
int32_t sx_scopes::init(void)
{
        int32_t l_s;
        // -------------------------------------------------
        // engine
        // -------------------------------------------------
        m_engine = new ns_waflz::engine();
        l_s = m_engine->init();
        if(l_s != WAFLZ_STATUS_OK)
        {
                NDBG_PRINT("error initializing engine\n");
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // create scope configs
        // -------------------------------------------------
        m_scopes_configs = new ns_waflz::scopes_configs(*m_engine, false);
        if(l_s != WAFLZ_STATUS_OK)
        {
                // TODO log error
                return STATUS_ERROR;
        }
        // -------------------------------------------------
        // load scopes dir
        // -------------------------------------------------
        if(m_scopes_dir)
        {
                l_s = m_scopes_configs->load_scopes_dir(m_config.c_str(), m_config.length());
                if(l_s != WAFLZ_STATUS_OK)
                {
                        NDBG_PRINT("error read dir %s\n", m_config.c_str());
                        return STATUS_ERROR;
                }
        }
        // -------------------------------------------------
        // load single scopes file
        // -------------------------------------------------
        else
        {
                char *l_buf = NULL;
                uint32_t l_buf_len = 0;
                //NDBG_PRINT("reading file: %s\n", l_instance_file.c_str());
                l_s = ns_waflz::read_file(m_config.c_str(), &l_buf, l_buf_len);
                if(l_s != WAFLZ_STATUS_OK)
                {
                        NDBG_PRINT("error read_file: %s\n", m_config.c_str());
                        return STATUS_ERROR;
                }
                l_s = m_scopes_configs->load_scopes(l_buf, l_buf_len);
                if(l_s != WAFLZ_STATUS_OK)
                {
                        NDBG_PRINT("error loading config: %s. reason: %s\n", m_config.c_str(), m_scopes_configs->get_err_msg());
                        if(l_buf) { free(l_buf); l_buf = NULL; }
                        return STATUS_ERROR;
                }
                if(l_buf) { free(l_buf); l_buf = NULL; l_buf_len = 0;}
        }
        // -------------------------------------------------
        // update 
        // -------------------------------------------------
        m_update_scopes_h = new update_scopes_h();
        m_update_scopes_h->m_bg_load = m_bg_load;
        m_lsnr->add_route("/update_instance", m_update_scopes_h);
        return STATUS_OK;
}
//: ----------------------------------------------------------------------------
//: \details: TODO
//: \return:  TODO
//: \param:   TODO
//: ----------------------------------------------------------------------------
ns_is2::h_resp_t sx_scopes::handle_rqst(const waflz_pb::enforcement **ao_enf,
                                        ns_waflz::rqst_ctx **ao_ctx,
                                        ns_is2::session &a_session,
                                        ns_is2::rqst &a_rqst,
                                        const ns_is2::url_pmap_t &a_url_pmap)
{
        ns_is2::h_resp_t l_resp_code = ns_is2::H_RESP_NONE;
        if(ao_enf) { *ao_enf = NULL;}
        if(!m_scopes_configs)
        {
                return ns_is2::H_RESP_SERVER_ERROR;
        }
        // -------------------------------------------------
        // events
        // -------------------------------------------------
        int32_t l_s;
        waflz_pb::event *l_event_prod = NULL;
        waflz_pb::event *l_event_audit = NULL;
        ns_waflz::rqst_ctx *l_ctx  = NULL;
        m_resp = "{\"status\": \"ok\"}";
        // -------------------------------------------------
        // process scopes dir/single scopes file
        // -------------------------------------------------
        if(m_scopes_dir)
        {
                // -------------------------------------------------
                // get cust an
                // -------------------------------------------------
                std::string l_id;
                const ns_is2::mutable_data_map_list_t& l_headers(a_rqst.get_header_map());
                ns_is2::mutable_data_t i_hdr;
                if(!ns_is2::find_first(i_hdr,
                                       l_headers,
                                       _SCOPEZ_SERVER_SCOPES_ID,
                                       sizeof(_SCOPEZ_SERVER_SCOPES_ID)))
                {
                        NDBG_PRINT("an was not provided in the header waf-scopes-id\n");
                        return ns_is2::H_RESP_SERVER_ERROR; 
                }
                l_id.assign(i_hdr.m_data, i_hdr.m_len);
                uint64_t l_cust_id;
                l_s = ns_waflz::convert_hex_to_uint(l_cust_id, l_id.c_str());
                if(l_s != WAFLZ_STATUS_OK)
                {
                        NDBG_PRINT("an provided is not a provided hex\n");
                        return ns_is2::H_RESP_SERVER_ERROR;
                }
                l_s = m_scopes_configs->process(ao_enf,
                                                &l_event_audit,
                                                &l_event_prod,
                                                &a_session,
                                                l_cust_id,
                                                &l_ctx);
                // -------------------------------------------------
                // process
                // -------------------------------------------------
                if(l_s != WAFLZ_STATUS_OK)
                {
                        NDBG_PRINT("error processing config. reason: %s\n",
                                    m_scopes_configs->get_err_msg());
                        if(l_event_audit) { delete l_event_audit; l_event_audit = NULL; }
                        if(l_event_prod) { delete l_event_prod; l_event_prod = NULL; }
                        if(l_ctx) { delete l_ctx; l_ctx = NULL; }
                        return ns_is2::H_RESP_SERVER_ERROR;
                }
        }
        else
        {
                ns_waflz::scopes* l_scopes = NULL;
                l_scopes = m_scopes_configs->get_first_scopes();
                if(l_scopes == NULL)
                {
                        NDBG_PRINT("No scopes to process\n");
                        return ns_is2::H_RESP_SERVER_ERROR;
                }
                // -------------------------------------------------
                // process
                // -------------------------------------------------
                l_s = l_scopes->process(ao_enf, &l_event_audit, &l_event_prod, &a_session, &l_ctx);
                if(l_s != WAFLZ_STATUS_OK)
                {
                        NDBG_PRINT("error processing config. reason: %s\n",
                                    l_scopes->get_err_msg());
                        if(l_event_audit) { delete l_event_audit; l_event_audit = NULL; }
                        if(l_event_prod) { delete l_event_prod; l_event_prod = NULL; }
                        if(l_ctx) { delete l_ctx; l_ctx = NULL; }
                        return ns_is2::H_RESP_SERVER_ERROR;
                }
        }
        // -------------------------------------------------
        // *************************************************
        //                R E S P O N S E
        // *************************************************
        // -------------------------------------------------
        std::string l_event_str = "{}";
        // -------------------------------------------------
        // for instances create string with both...
        // -------------------------------------------------
        rapidjson::Document l_event_prod_json;
        rapidjson::Document l_event_audit_json;
        if(l_event_audit)
        {
                l_s = ns_waflz::convert_to_json(l_event_audit_json, *l_event_audit);
                if(l_s != JSPB_OK)
                {
                        NDBG_PRINT("error performing convert_to_json.\n");
                        if(l_event_audit) { delete l_event_audit; l_event_audit = NULL; }
                        if(l_event_prod) { delete l_event_prod; l_event_prod = NULL; }
                        if(l_ctx) { delete l_ctx; l_ctx = NULL; }
                        return ns_is2::H_RESP_SERVER_ERROR;
                }
                if(l_event_audit) { delete l_event_audit; l_event_audit = NULL; }
        }
        if(l_event_prod)
        {
                l_s = ns_waflz::convert_to_json(l_event_prod_json, *l_event_prod);
                if(l_s != JSPB_OK)
                {
                        NDBG_PRINT("error performing convert_to_json.\n");
                        if(l_event_audit) { delete l_event_audit; l_event_audit = NULL; }
                        if(l_event_prod) { delete l_event_prod; l_event_prod = NULL; }
                        if(l_ctx) { delete l_ctx; l_ctx = NULL; }
                        return ns_is2::H_RESP_SERVER_ERROR;
                }
        }
        rapidjson::Document l_js_doc;
        l_js_doc.SetObject();
        rapidjson::Document::AllocatorType& l_js_allocator = l_js_doc.GetAllocator();
        l_js_doc.AddMember("audit_profile", l_event_audit_json, l_js_allocator);
        l_js_doc.AddMember("prod_profile",  l_event_prod_json, l_js_allocator);
        rapidjson::StringBuffer l_strbuf;
        rapidjson::Writer<rapidjson::StringBuffer> l_js_writer(l_strbuf);
        l_js_doc.Accept(l_js_writer);
        m_resp.assign(l_strbuf.GetString(), l_strbuf.GetSize());
        // -------------------------------------------------
        // cleanup
        // -------------------------------------------------
        if(l_event_audit) { delete l_event_audit; l_event_audit = NULL; }
        if(l_event_prod) { delete l_event_prod; l_event_prod = NULL; }
        if(ao_ctx)
        {
                *ao_ctx = l_ctx;
        }
        else if(l_ctx)
        {
                delete l_ctx; l_ctx = NULL;
        }
        return l_resp_code;
}
}
// -*- C++ -*-

//=============================================================================
/**
 *  @file   SSL_Asynch_Stream.h
 *
 *  SSL_Asynch_Stream.h,v 1.4 2002/08/29 03:33:01 shuston Exp
 *
 *  @author Alexander Libman <alibman@baltimore.com>
 *
 */
//=============================================================================

#ifndef ACE_SSL_ASYNCH_STREAM_H 
#define ACE_SSL_ASYNCH_STREAM_H 

#include "ace/pre.h"

#include "ace/SSL/SSL_Export.h"
#include "ace/SSL/SSL_Context.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if OPENSSL_VERSION_NUMBER > 0x0090581fL && ((defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)) || (defined (ACE_HAS_AIO_CALLS)) || (defined (ACE_HAS_AIO_EMULATION)))

#include "ace/Asynch_IO.h"
#include "ace/Message_Block.h"
#include "ace/Synch_T.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

extern "C"
{
  BIO_METHOD * BIO_s_ACE_Asynch (void);
  BIO * BIO_new_ACE_Asynch (void *ssl_asynch_stream);
}



/**
 * @class ACE_SSL_Asynch_Stream
 *
 * @brief This class is a factory for starting off asynchronous reads
 * on a stream. This class forwards all methods to its
 * implementation class.
 * @par
 * Once open() is called, multiple asynchronous read()s can
 * started using this class.  An ACE_SSL_Asynch_Stream::Result
 * will be passed back to the @param handler when the asynchronous
 * reads completes through the ACE_Handler::handle_read_stream
 * callback.
 */
class ACE_SSL_Export ACE_SSL_Asynch_Stream
  : public ACE_Asynch_Operation,
    public ACE_Handler 
{
  friend int ACE_Asynch_BIO_read (BIO * pBIO, char * buf, int len);
  friend int ACE_Asynch_BIO_write (BIO * pBIO, const char * buf, int len);

public:

  enum Stream_Type
    {
      ST_CLIENT = 0x0001,
      ST_SERVER = 0x0002
    };


  /// The constructor.

  ACE_SSL_Asynch_Stream (Stream_Type type = ST_SERVER,
                         ACE_SSL_Context *context = 0);

  /// Destructor
  virtual ~ACE_SSL_Asynch_Stream (void);

  /**
   *  get supported operations mask 
   *  see ACE_Asynch_Result::OpCode
   */
  virtual int supported_operations (void) const;


  virtual int cancel(void);

  virtual int close (void);

  int open (ACE_Handler &handler,
            ACE_HANDLE handle = ACE_INVALID_HANDLE,
            const void *completion_key = 0,
            ACE_Proactor *proactor = 0,
            int pass_ownership = 0);


  int read (ACE_Message_Block &message_block,
            size_t num_bytes_to_read,
            const void *act = 0,
            int priority = 0,
            int signal_number = ACE_SIGRTMIN);

  int write (ACE_Message_Block &message_block,
             size_t bytes_to_write,
             const void *act = 0,
             int priority = 0,
             int signal_number = ACE_SIGRTMIN);

protected:

  /// virtual from ACE_Handler

  /// This method is called when BIO write request is completed. It
  /// processes the IO completion and calls do_SSL_state_machine().
  virtual void handle_write_stream
               (const ACE_Asynch_Write_Stream::Result &result);

  /// This method is called when BIO read request is completed. It
  /// processes the IO completion and calls do_SSL_state_machine().
  virtual void handle_read_stream
               (const ACE_Asynch_Read_Stream::Result &result);

  /// This method is called when all SSL sessions are closed and no
  /// more pending AIOs exist.  It also calls users handle_wakeup().
  virtual void handle_wakeup (void);

private:

  /**
   * @name SSL State Machine
   */
  //@{
  int do_SSL_state_machine (int flg_cb);
  int do_SSL_handshake (void);
  int do_SSL_read (int flg_cb);
  int do_SSL_write(int flg_cb);
  int do_SSL_shutdown(int flg_cb);
  //@}

  void print_error (int err_ssl,
                    const ACE_TCHAR *pText);

  int pending_BIO_count (void);

  /// This method is called to notify user handler when user's read in
  /// done.
  int notify_read (int flg_cb, int bytes_transferred, int error);

  /// This method is called to notify user handler when user's write
  /// in done.
  int notify_write (int flg_cb, int bytes_transferred, int error);

  /// This method is called to notify ourself that SSL session is
  /// shutdown and that there is no more I/O activity now and in the
  /// future.
  int notify_close (void);
  int close_handle (void);
  
  /**
   * @name BIO Helpers
   */
  //@{
  int ssl_bio_read (char * buf, size_t len, int & errval);
  int ssl_bio_write (const char * buf, size_t len, int & errval);
  //@}

  void bio_cancel();

protected:

  /// Stream Type ST_CLIENT/ST_SERVER
  Stream_Type type_;

  /// The real file/socket handle
  ACE_HANDLE handle_;
  int        owner_;

  /// The proactor
  ACE_Proactor * proactor_;


  /// External, i.e. read result faked for user
  ACE_Asynch_Read_Stream_Result    ext_read_result_ ;
  
  /// number pending external(user) reads
  /// currently supported only 1, may be extended later
  int                              num_ext_reads_;

  /// External, i.e. write result faked for user
  ACE_Asynch_Write_Stream_Result   ext_write_result_ ;

  /// number pending external(user) writes
  /// currently supported only 1, may be extended later
  int                              num_ext_writes_;

  /// Stream state/flags
  enum Stream_Flag
    {
      SF_STREAM_OPEN    = 0x0001, /// istream_ open OK
      SF_REQ_SHUTDOWN   = 0x0002, /// request to SSL shutdown
      SF_SHUTDOWN_DONE  = 0x0004, /// SSL shutdown finished
      SF_CLOSE_NTF_SENT = 0x0008, /// Close notification sent
      SF_DELETE_ENABLE  = 0x0010, /// Stream can be safely destroyed
      SF_IN_CALLBACK    = 0x0020  /// Stream in internal callback
    };

  int flags_;

  /// The SSL session.
  SSL * ssl_;

  /// The BIO implementation
  BIO * bio_;

  /// The real streams which work under the ssl connection.
  /// BIO performs I/O via this streams

  enum BIO_Flag  // internal IO flags
    {
      BF_EOS    = 0x01,   // end of stream
      BF_AIO    = 0x02,   // real AIO in progress
      BF_CANCEL = 0x04    // BIO cancelled
    };

  /**
   * @name Internal stream, buffer and info for BIO read
   */
  //@{
  ACE_Asynch_Read_Stream  bio_istream_;
  ACE_Message_Block       bio_inp_msg_;
  int                     bio_inp_errno_;
  int                     bio_inp_flag_;
  //@}

  /**
   * @name Internal stream, buffer and info for BIO write
   */
  //@{
  ACE_Asynch_Write_Stream bio_ostream_;
  ACE_Message_Block       bio_out_msg_;
  int                     bio_out_errno_;
  int                     bio_out_flag_;
  //@}

  /// Mutex to protect work
  ACE_SYNCH_MUTEX mutex_;

};


ACE_END_VERSIONED_NAMESPACE_DECL

#endif  /* OPENSSL_VERSION_NUMBER > 0x0090581fL && (ACE_WIN32 ||
           ACE_HAS_AIO_CALLS) */

#include "ace/post.h"

#endif //TPROACTOR_SSL_ASYNCH_STREAM_H 

/*
	Copyright (c) 2012 Christopher A. Taylor.  All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* Neither the name of LibCat nor the names of its contributors may be used
	  to endorse or promote products derived from this software without
	  specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CAT_WIREHAIR_HPP
#define CAT_WIREHAIR_HPP

/*
	Wirehair Streaming Forward Error Correction

		Wirehair is an FEC codec used to improve reliability of data sent
	over a Binary Erasure Channel (BEC) such as satellite Internet or WiFi.
	The data to transmit over the BEC is encoded into equal-sized blocks.
	When enough blocks are received at the other end of the channel, then
	the original data can be recovered.  How many additional blocks are
	needed is random, though the number of additional blocks is low and
	does not vary based on the size of the data.
*/

#include "WirehairDetails.hpp"

namespace cat {

namespace wirehair {


/*
	Wirehair FEC Encoder
*/
class Encoder : protected Codec
{
public:
	CAT_INLINE u32 BlockCount() { return Codec::BlockCount(); }

	// Attempt to initialize with the given message size and number of bytes per block
	// message_in: Has message_bytes
	CAT_INLINE Result BeginEncode(const void *message_in, int message_bytes, int block_bytes)
	{
		Result r = Codec::InitializeEncoder(message_bytes, block_bytes);
		if (r == R_WIN) r = Codec::EncodeFeed(message_in);
		return r;
	}

	// Generate an encoded block
	// id : Block number, id < BlockCount are same as original message
	// block_out : Has block_bytes
	CAT_INLINE void Encode(u32 id, void *block_out)
	{
		return Codec::Encode(id, block_out);
	}
};


/*
	Wirehair FEC Decoder
*/
class Decoder : protected Codec
{
	void *_message_out;

public:
	CAT_INLINE u32 BlockCount() { return Codec::BlockCount(); }

	// Attempt to initialize the codec with the given message size and block bytes
	CAT_INLINE Result BeginDecode(void *message_out, int message_bytes, int block_bytes)
	{
		// Remember output message location
		_message_out = message_out;

		return Codec::InitializeDecoder(message_bytes, block_bytes);
	}

	// Feed decoder a block
	// id: Block number, id < BlockCount are same as original message
	// block_in: Has block_bytes
	CAT_INLINE Result Decode(u32 id, const void *block_in)
	{
		Result r = Codec::DecodeFeed(id, block_in);
		if (r == R_WIN) ReconstructOutput(_message_out);
		return r; // Return R_WIN when message has been reconstructed
	}
};


} // namespace wirehair

} // namespace cat

#endif // CAT_WIREHAIR_HPP

/*
* This file was generated automatically by xsubpp version 1.9508 from the
* contents of tmp. Do not edit this file, edit tmp instead.
*
*		ANY CHANGES MADE HERE WILL BE LOST!
*
*/


/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2004 EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
		are required to give you total support for your newly bought product;
		without even the implied warranty of MERCHANTABILITY or FITNESS FOR
		A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

typedef const char Const_char;

#ifdef EMBPERL
#include "global_define.h"
#include "useperl.h"
#include "eqdb_res.h"

#pragma warning( disable : 4267 )

XS(XS_EQDBRes_num_rows); /* prototype to pass -Wmissing-prototypes */
XS(XS_EQDBRes_num_rows)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: EQDBRes::num_rows(THIS)");
	{
		EQDBRes *		THIS;
		unsigned long		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "EQDBRes"))
		{
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(EQDBRes *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type EQDBRes");
		if(THIS == nullptr)
			Perl_croak(aTHX_ "THIS is nullptr, avoiding crash.");

		RETVAL = THIS->num_rows();
		XSprePUSH; PUSHu((UV)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_EQDBRes_num_fields); /* prototype to pass -Wmissing-prototypes */
XS(XS_EQDBRes_num_fields)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: EQDBRes::num_fields(THIS)");
	{
		EQDBRes *		THIS;
		unsigned long		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "EQDBRes"))
		{
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(EQDBRes *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type EQDBRes");
		if(THIS == nullptr)
			Perl_croak(aTHX_ "THIS is nullptr, avoiding crash.");

		RETVAL = THIS->num_fields();
		XSprePUSH; PUSHu((UV)RETVAL);
	}
	XSRETURN(1);
}

XS(XS_EQDBRes_DESTROY); /* prototype to pass -Wmissing-prototypes */
XS(XS_EQDBRes_DESTROY)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: EQDBRes::DESTROY(THIS)");
	{
		EQDBRes *		THIS;

		if (SvROK(ST(0)))
		{
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(EQDBRes *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not a reference");
		if(THIS == nullptr)
			Perl_croak(aTHX_ "THIS is nullptr, avoiding crash.");

		delete THIS;
	}
	XSRETURN_EMPTY;
}

XS(XS_EQDBRes_finish); /* prototype to pass -Wmissing-prototypes */
XS(XS_EQDBRes_finish)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: EQDBRes::finish(THIS)");
	{
		EQDBRes *		THIS;

		if (sv_derived_from(ST(0), "EQDBRes"))
		{
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(EQDBRes *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type EQDBRes");
		if(THIS == nullptr)
			Perl_croak(aTHX_ "THIS is nullptr, avoiding crash.");

		THIS->finish();
	}
	XSRETURN_EMPTY;
}

XS(XS_EQDBRes_fetch_row_array); /* prototype to pass -Wmissing-prototypes */
XS(XS_EQDBRes_fetch_row_array)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: EQDBRes::fetch_row_array(THIS)");
	{
		EQDBRes *		THIS;
		std::vector<std::string>		RETVAL;

		if (sv_derived_from(ST(0), "EQDBRes"))
		{
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(EQDBRes *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type EQDBRes");
		if(THIS == nullptr)
			Perl_croak(aTHX_ "THIS is nullptr, avoiding crash.");

		RETVAL = THIS->fetch_row_array();
		ST(0) = sv_newmortal();
		{
			U32 ix_RETVAL;
			/* pop crap off the stack we don't really want */
			POPs;
			POPs;
			/* grow the stack to the number of elements being returned */
			EXTEND(SP, RETVAL.size());
			for (ix_RETVAL = 0; ix_RETVAL < RETVAL.size(); ix_RETVAL++) {
					const std::string &it = RETVAL[ix_RETVAL];
					ST(ix_RETVAL) = sv_newmortal();
					sv_setpvn(ST(ix_RETVAL), it.c_str(), it.length());
			}
			/* hackish, but I'm over it. The normal xsubpp return will be right below this */
			XSRETURN(RETVAL.size());
		}
	}
	XSRETURN(1);
}

XS(XS_EQDBRes_fetch_row_hash); /* prototype to pass -Wmissing-prototypes */
XS(XS_EQDBRes_fetch_row_hash)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: EQDBRes::fetch_row_hash(THIS)");
	{
		EQDBRes *		THIS;
		std::map<std::string,std::string>		RETVAL;

		if (sv_derived_from(ST(0), "EQDBRes"))
		{
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(EQDBRes *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type EQDBRes");
		if(THIS == nullptr)
			Perl_croak(aTHX_ "THIS is nullptr, avoiding crash.");

		RETVAL = THIS->fetch_row_hash();
		ST(0) = sv_newmortal();
		if (RETVAL.begin()!=RETVAL.end())
		{
			//NOTE: we are leaking the original ST(0) right now
			HV *hv = newHV();
			sv_2mortal((SV*)hv);
			ST(0) = newRV((SV*)hv);

			std::map<std::string,std::string>::const_iterator cur, end;
			cur = RETVAL.begin();
			end = RETVAL.end();
			for(; cur != end; cur++)
			{
				/* get the element from the hash, creating if needed (will be needed) */
				SV**ele = hv_fetch(hv, cur->first.c_str(), cur->first.length(), TRUE);
				if(ele == nullptr)
				{
					Perl_croak(aTHX_ "Unable to create a hash element for RETVAL");
					break;
				}
				/* put our string in the SV associated with this element in the hash */
				sv_setpvn(*ele, cur->second.c_str(), cur->second.length());
			}
		}
	}
	XSRETURN(1);
}

XS(XS_EQDBRes_fetch_lengths); /* prototype to pass -Wmissing-prototypes */
XS(XS_EQDBRes_fetch_lengths)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: EQDBRes::fetch_lengths(THIS)");
	{
		EQDBRes *		THIS;
		unsigned long *		RETVAL;
		dXSTARG;

		if (sv_derived_from(ST(0), "EQDBRes"))
		{
			IV tmp = SvIV((SV*)SvRV(ST(0)));
			THIS = INT2PTR(EQDBRes *,tmp);
		}
		else
			Perl_croak(aTHX_ "THIS is not of type EQDBRes");
		if(THIS == nullptr)
			Perl_croak(aTHX_ "THIS is nullptr, avoiding crash.");

		RETVAL = THIS->fetch_lengths();
		XSprePUSH; PUSHp((char *)RETVAL, sizeof(*RETVAL));
	}
	XSRETURN(1);
}

#ifdef __cplusplus
extern "C"
#endif
XS(boot_EQDBRes); /* prototype to pass -Wmissing-prototypes */
XS(boot_EQDBRes)
{
	dXSARGS;
	char file[256];
	strncpy(file, __FILE__, 256);
	file[255] = 0;

	if(items != 1)
		fprintf(stderr, "boot_quest does not take any arguments.");
	char buf[128];

	//add the strcpy stuff to get rid of const warnings....

	XS_VERSION_BOOTCHECK ;

	newXSproto(strcpy(buf, "num_rows"), XS_EQDBRes_num_rows, file, "$");
	newXSproto(strcpy(buf, "num_fields"), XS_EQDBRes_num_fields, file, "$");
	newXSproto(strcpy(buf, "DESTROY"), XS_EQDBRes_DESTROY, file, "$");
	newXSproto(strcpy(buf, "finish"), XS_EQDBRes_finish, file, "$");
	newXSproto(strcpy(buf, "fetch_row_array"), XS_EQDBRes_fetch_row_array, file, "$");
	newXSproto(strcpy(buf, "fetch_row_hash"), XS_EQDBRes_fetch_row_hash, file, "$");
	newXSproto(strcpy(buf, "fetch_lengths"), XS_EQDBRes_fetch_lengths, file, "$");
	XSRETURN_YES;
}

#endif //EMBPERL_XS_CLASSES
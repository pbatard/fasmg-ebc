;
; EBC (EFI Byte Code) assembler definition for fasmg
; Copyright © 2016 Pete Batard <pete@akeo.ie>
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

element R

repeat 8 i:0
  element R#i?  : R + i
  element @R#i? : R + i + 0x80
end repeat

element FLAGS?  : R + 0x800
element IP?     : R + 0x801

;
; Supporting macros
;

macro di width, val
  if (width = 0)
    db val
  else if (width = 1)
    dw val
  else if (width = 2)
    dd val
  else
    dq val
  end if
end macro

macro abs abs_val, val
  if (val < 0)
    abs_val = -val
  else
    abs_val = val
  end if
end macro

macro is_relative ret, val
  local v
  v = val
  if (val relativeto $)
    ret = 1
  else
    ret = 0
  end if
end macro

macro is_immediate ret, ops&
  match a =, b =, c, ops
    ret = 0
  else match a =, b, ops
    ret = 0
  else match a ( b ), ops
    ret = 0
  else match v, ops
    local _v
    _v = +v
    if (_v relativeto 0)
      ret = 1
    else
      is_relative ret, _v
    end if
  else
    ret = 0
  end match
end macro

macro ebc.parse_register reg_num, ret, reg
  local value
  value = +reg
  reg_num = value metadata 1 - R
  if (reg_num >= 0x800)
    ret = 2 ; indicates a VM register
  else if (reg_num >= 0x80)
    ret = 1
  else
    ret = 0
  end if
  reg_num = (reg_num and 0x07)
end macro

macro ebc.get_index_size index_size, nat, cst
  local abs_cst, abs_nat, used_bits, max_n, nbits
  abs abs_cst, cst
  abs abs_nat, nat
  used_bits = 4 ; 4 bits for sign + natural width
  while (abs_cst > 0)
    used_bits = used_bits + 1
    abs_cst = abs_cst shr 1
  end while
  index_size = 16
  repeat 3 i:1
    nbits = (1 shl i) ; factor for number of bits assigned to natural units
    if (index_size >= used_bits) & ((abs_nat = 0) | ((index_size - used_bits) >= nbits))
      max_n = nbits shl ((index_size - used_bits) and (not (nbits-1)))
      if (abs_nat < max_n)
        break
      end if
    end if
    index_size = index_size * 2
  end repeat
end macro

macro ebc.insert_index index_size, nat, cst
  local abs_cst, abs_nat, sign, width, nbits
  sign = 0
  nbits = index_size / 8
  if ((cst < 0) & (nat > 0)) | ((cst > 0) & (nat < 0))
     err 'constant and natural values must have the same sign'
  end if
  abs abs_cst, cst
  abs abs_nat, nat
  if (abs_cst <> cst)
    sign = 1
  end if
  width = 8
  if (abs_nat = 0)
    width = 0
  else
    repeat 7
      if (abs_nat < (1 shl (% * nbits)))
        width = %
        break
      end if
    end repeat
  end if
  if (width > 7) | ((width = 7) & (index_size = 16))
    err 'natural value too large for index size'
  end if
  if (abs_cst >= 1 shl (index_size - 4 - (width*nbits)))
    err 'constant and natural values too large for index size'
  end if
  ; NB: we abuse the fact that 'di' inserts a 'dq' for size 3 or 4
  di (index_size/16), sign shl (index_size-1) + width shl (index_size-4) + abs_cst shl (width*nbits) + abs_nat
end macro

macro ebc.parse_parenthesis index, nat, cst, imm, ops&
  match n =, c, ops
    index = 1
    nat = n
    cst = c
    imm = 0
  else
    local is_imm
    is_immediate is_imm, ops
    if (is_imm)
      index = 3
      nat = 0
      cst = 0
      imm = ops
    else
      err 'not an immediate value'
    end if
  end match
end macro

macro ebc.move_imm_ind_rel mov_width, op2_width, ops&
  local op1, op2, op1_reg_num, op1_indirect, op1_index, op2_index, index_size, op1_imm
  local op2_imm, op1_nat, op2_nat, op1_cst, op2_cst, opcode, ins_width, _op2_width, op2_rel
  if (mov_width >= 0)
    opcode = 0x37 ; MOVI
    ins_width = mov_width
  else if (mov_width = -1)
    opcode = 0x38 ; MOVIn
    ins_width = 0
  else if (mov_width = -2)
    opcode = 0x39 ; MOVREL
    ins_width = 0
  end if
  op1_index = 0
  op2_index = 0
  op1_nat = 0
  op1_cst = 0
  op2_rel = 0

  match _op1 (val1) =, (val2), ops
    if (mov_width <> -1)
      err 'syntax error'
    end if
    define op1 _op1
    ebc.parse_parenthesis op1_index, op1_nat, op1_cst, op1_imm, val1
    if (op1_index <> 1)
      err 'syntax error'
    end if
    ebc.parse_parenthesis op2_index, op2_nat, op2_cst, op2_imm, val2
    if (op2_index <> 1)
      err 'syntax error'
    end if
  else match _op1 (val1) =, _op2, ops
    define op1 _op1
    ebc.parse_parenthesis op1_index, op1_nat, op1_cst, op1_imm, val1
    if (op1_index <> 1)
      err 'syntax error'
    end if
    op2 = _op2
  else match _op1 =, (val2), ops
    if (mov_width <> -1)
      err 'syntax error'
    end if
    define op1 _op1
    ebc.parse_parenthesis op2_index, op2_nat, op2_cst, op2_imm, val2
    if (op2_index <> 1)
      err 'syntax error'
    end if
  else match _op1 =, _op2, ops
    define op1 _op1
    op2 = _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect, op1
  if (op1_indirect > 1)
    err 'illegal register'
  end if
  if (op1_index = 1) & (op1_indirect <> 1)
    err 'illegal direct register indexed operation'
  end if
  if (op1_nat = 0) & (op1_cst = 0)
    op1_index = 0
  end if
  _op2_width = op2_width
  if (opcode <> 0x38)
    is_relative op2_rel, op2
  end if
  if (_op2_width = -1)
    if (opcode = 0x38) ; MOVIn
      ebc.get_index_size index_size, op2_nat, op2_cst
      if (index_size > 64)
        err 'constant and natural values too large for index encoding'
      end if
      repeat 3 i:1
        if index_size = (8 shl i)
          _op2_width = i
          break
        end if
      end repeat
    else
      if (opcode = 0x39) ; MOVREL
        local _op2
        _op2 = op2
        repeat 3 i:1
          if (op2_rel) ; account for the instruction size
            _op2 = op2 - $ - 2 - (2*op1_index) - (1 shl i)
          end if
          ; eg: (_op2 >= -0x8000) & (_op2 < 0x8000)
          if (_op2 >= -(1 shl ((8 shl i)-1))) & (_op2 < (1 shl ((8 shl i)-1)))
            _op2_width = i
            op2 = _op2
            break;
          end if
        end repeat
        if (_op2_width < 0)
          err 'value is too large'
        end if
      else if (op2_rel)
        _op2_width = 3
      else if (op2 >= -0x8000) & (op2 < 0x10000) ; NB: We assume that positive immediates => unsigned operations
        _op2_width = 1
      else if (op2 >= -0x80000000) & (op2 < 0x100000000)
        _op2_width = 2
      else
         if (op2 < -0x8000000000000000) | (op2 > 0x10000000000000000)
           err 'value is too large'
         end if
        _op2_width = 3
      end if
    end if
  else if (opcode = 0x39) & (op2_rel) ; MOVREL
    ; Compute the offset
    op2 = op2 - $ - 2 - (2*op1_index) - (1 shl _op2_width)
  end if

  db _op2_width shl 6 + opcode
  db op1_index shl 6 + ins_width shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op1_index = 1)
    ebc.insert_index 16, op1_nat, op1_cst
  end if
  if (opcode = 0x38) ; MOVIn
    ebc.insert_index (1 shl _op2_width)*8, op2_nat, op2_cst
  else
    if (op2_rel)
      if (op2_width > 0) & (op2_width < 2)
        err 'move size is too small'
      end if
    else if ((op2 < -(1 shl ((8 shl _op2_width)-1))) | (op2 >= (1 shl (8 shl _op2_width))))
      err 'value is too large'
    end if
    di _op2_width, op2
  end if
end macro

macro ebc.move_general opcode, ops&
  local op1, op2, op1_reg_num, op2_reg_num, op1_indirect, op2_indirect, op1_imm, op2_imm
  local op1_index, op2_index, op1_nat, op2_nat, op1_cst, op2_cst, _opcode
  local index_size, index_size1, index_size2
  op1_index = 0
  op2_index = 0
  op1_nat = 0
  op2_nat = 0
  op1_cst = 0
  op2_cst = 0
  op1_imm = 0
  op2_imm = 0
  index_size = 16
  _opcode = opcode and 0xff

  match _op1 (val1) =, _op2 (val2), ops
    define op1 _op1
    ebc.parse_parenthesis op1_index, op1_nat, op1_cst, op1_imm, val1
    if (op1_index = 3)
      err 'syntax error'
    end if
    define op2 _op2
    ebc.parse_parenthesis op2_index, op2_nat, op2_cst, op2_imm, val2
    if (op2_index = 3) & (_opcode <> 0x25) & (_opcode <> 0x26)
      err 'syntax error'
    end if
  else match _op1 (val1) =, _op2, ops
    define op1 _op1
    ebc.parse_parenthesis op1_index, op1_nat, op1_cst, op1_imm, val1
    if (op1_index = 3)
      err 'syntax error'
    end if
    define op2 _op2
  else match _op1 =, _op2 (val2), ops
    define op1 _op1
    define op2 _op2
    ebc.parse_parenthesis op2_index, op2_nat, op2_cst, op2_imm, val2
    if (op2_index = 3) & (_opcode <> 0x25) & (_opcode <> 0x26)
      err 'syntax error'
    end if
  else match _op1 =, _op2, ops
    define op1 _op1
    define op2 _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect, op1
  ebc.parse_register op2_reg_num, op2_indirect, op2
  if (op1_indirect > 1) | (op2_indirect > 1)
    err 'illegal register'
  end if
  if (op1_index = 1) & (op1_indirect <> 1)
    err 'illegal direct register indexed operation'
  else if (op2_index = 3) & (op2_indirect = 1)
    err 'immediate value specified for indirect register operation'
  end if
  if (op1_nat = 0) & (op1_cst = 0)
    op1_index = 0
  end if
  if (op2_index = 1) & (op2_nat = 0) & (op2_cst = 0)
    op2_index = 0
  end if
  if ((opcode >= 0x21) & (opcode <= 0x24)) | (opcode = 0x26) | (opcode = 0x33)
    index_size = 32
  else if (opcode = 0x28)
    index_size = 64
  else if (opcode > 0x100) & ((op1_index = 1) | (op2_index = 1))
    index_size1 = 0
    index_size2 = 0
    if (op1_index = 1)
      ebc.get_index_size index_size1, op1_nat, op1_cst
    end if
    if (op2_index = 1)
      ebc.get_index_size index_size2, op2_nat, op2_cst
    end if
    if (index_size1 < index_size2)
      index_size = index_size2
    else
      index_size = index_size1
    end if
    if (index_size = 64) & (opcode = 0x120)
      _opcode = 0x28
    else if (index_size = 32) & (opcode >= 0x11D) & (opcode <= 0x120)
      _opcode = _opcode + 4 ; switch _opcode to the 'd' version
    else if (index_size = 32) & ((opcode = 0x132) | (opcode = 0x125))
      _opcode = _opcode + 1 ; switch _opcode to the 'd' version
    else if (index_size = 16) & (((opcode >= 0x11D) & (opcode <= 0x120)) | (opcode = 0x132) | (opcode = 0x125))
      ; nothing to do - _opcode is already set for 'w' version
    else
      err 'constant and natural values too large for index encoding'
    end if
  end if

  db op1_index shl 7 + (op2_index and 1) shl 6 + _opcode
  db op2_indirect shl 7 + op2_reg_num shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op1_index = 1)
    ebc.insert_index index_size, op1_nat, op1_cst
  end if
  if (op2_index = 1)
    ebc.insert_index index_size, op2_nat, op2_cst
  else if (op2_index = 3)
    di (opcode - 0x24), op2_imm
  end if
end macro

macro ebc.math_general opcode, ops&
  local op1, op2, op1_reg_num, op2_reg_num, op1_indirect, op2_indirect
  local op2_index, op2_nat, op2_cst, op2_imm, op2_imm_rel
  op2_index = 0
  op2_nat = 0
  op2_cst = 0
  op2_imm = 0

  match _op1 =, _op2 (val), ops
    define op1 _op1
    define op2 _op2
    ebc.parse_parenthesis op2_index, op2_nat, op2_cst, op2_imm, val
  else match _op1 =, _op2, ops
    define op1 _op1
    define op2 _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect, op1
  ebc.parse_register op2_reg_num, op2_indirect, op2
  if (op1_indirect > 1) | (op2_indirect > 1)
    err 'illegal register'
  end if
  if (opcode >= 0x05) & (opcode <= 0x09) & (op1_indirect = 1)
    err 'illegal indirect on first operand'
  end if
  if (op2_index = 3) & (op2_indirect = 1)
    err 'immediate value specified for indirect register operation'
  end if
  is_relative op2_imm_rel, op2_imm
  if (~op2_imm_rel) & (op2_nat = 0) & (op2_cst = 0) & (op2_imm = 0)
    op2_index = 0
  end if

  db (op2_index and 1) shl 7 + opcode
  db op2_indirect shl 7 + op2_reg_num shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op2_index = 1)
    ebc.insert_index 16, op2_nat, op2_cst
  else if (op2_index = 3)
    dw op2_imm
  end if
end macro

macro ebc.push_pop opcode, ops&
  local op1, op1_reg_num, op1_indirect, op1_index
  local op1_nat, op1_cst, op1_imm, op1_rel, index_size
  op1_nat = 0
  op1_cst = 0
  op1_imm = 0

  match _op1 (val), ops
    ebc.parse_parenthesis op1_index, op1_nat, op1_cst, op1_imm, val
    define op1 _op1
  else
    op1_index = 0
    define op1 ops
  end match

  ebc.parse_register op1_reg_num, op1_indirect, op1
  if (op1_indirect > 1)
    err 'illegal register'
  end if
  if (op1_nat = 0) & (op1_cst = 0) & (op1_imm = 0)
    op1_index = 0
  end if
  if (op1_index = 3) & (op1_indirect = 1)
    err 'immediate value specified for indirect register operation'
  end if

  db (op1_index and 1) shl 7 + opcode
  db op1_indirect shl 3 + op1_reg_num
  if (op1_index = 1)
    ebc.insert_index 16, op1_nat, op1_cst
  else if (op1_index = 3)
    dw op1_imm
  end if
end macro

macro ebc.jump_get_offset jmp_size, rel, offset, ops&
  local min_jmp_size, addr
  min_jmp_size = jmp_size
  rel = 0

  match reg (n =, c), ops
    ; indexed => no offset
    jmp_size = -1
  else match reg (imm), ops
    addr = imm
    jmp_size = 0
  else match imm, ops
    local is_imm
    is_immediate is_imm, imm
    if (is_imm)
      addr = imm
      jmp_size = 0
    else
      ; register => no offset
      jmp_size = -1
    end if
  else
    err 'internal error'
  end match

  if (jmp_size = 0) ; we have a value for which to compute the offset
    is_relative rel, addr
    if (rel) ; relative address
      disp = addr - $
      if (disp and 0x1)
        err 'target address must be aligned to word boundary'
      end if
      if ((disp/2 - 1) >= -0x80) & ((disp/2 - 1) < 0x80) & (min_jmp_size < 32)
        jmp_size = 8
        offset = disp/2 - 1
      else if ((disp - 6) >= -0x80000000) & ((disp - 6) < 0x80000000) & (min_jmp_size < 64)
        jmp_size = 32
        offset = disp - 6
      else
        if ((disp - 10) < -0x8000000000000000) | ((disp - 10) >= 0x8000000000000000)
          err 'value is too large'
        end if
        jmp_size = 64
        offset = disp - 10
      end if
    else ; absolute address
      offset = addr
      if (addr >= -0x80000000) & (addr < 0x80000000)
        jmp_size = 32
      else
        jmp_size = 64
      end if
    end if
  end if
end macro

macro ebc.jump_call opcode, opcode_flags, ops&
  local op1, op1_reg_num, op1_indirect, op1_index
  local op1_nat, op1_cst, op1_imm, op1_rel, jmp_size
  op1_index = 0
  op1_nat = 0
  op1_cst = 0
  op1_rel = 0
  jmp_size = 32
  if (opcode and 0x40)
    jmp_size = 64
  end if

  match _op1 (val), ops
    define op1 _op1
    ebc.parse_parenthesis op1_index, op1_nat, op1_cst, op1_imm, val
    if (jmp_size = 64)
      err 'illegal register indexed operation'
    end if
  else
    local is_imm
    is_immediate is_imm, ops
    if (is_imm)
      if (jmp_size <> 64)
        err 'illegal immediate operation'
      end if
      op1_index = 3
      op1_imm = ops
      define op1 R0 ; need to define one, but will be ignored
    else
      define op1 ops
    end if
  end match

  if (op1_index = 3)
    ebc.jump_get_offset jmp_size, op1_rel, offset, op1_imm
    if (jmp_size <> 32) & (jmp_size <> 64)
      err 'internal error'
    else if ((jmp_size = 32) & (opcode and 0x40)) | ((jmp_size = 64) & (~ opcode and 0x40))
      err 'invalid jmp size'
    else if (opcode = 0x43) & (op1_rel)
      err 'illegal relative address'
    end if
  end if

  ebc.parse_register op1_reg_num, op1_indirect, op1
  if (op1_indirect > 1)
    err 'illegal register'
  end if
  if (op1_index = 1) & (op1_nat = 0) & (op1_cst = 0)
    op1_index = 0
  end if
  if (op1_index = 3) & (op1_indirect = 1)
    err 'immediate value specified for indirect register operation'
  end if

  db (op1_index and 1) shl 7 + opcode
  db (opcode_flags or op1_rel) shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op1_index = 1)
    ebc.insert_index 32, op1_nat, op1_cst
  else if (op1_index = 3)
    di jmp_size/16, offset
  end if
end macro

macro ebc.jump_short flags, addr
  local jmp_size, rel, offset
  jmp_size = 8
  ebc.jump_get_offset jmp_size, rel, offset, addr
  if (rel <> 1)
    err 'illegal absolute address'
  end if
  if (jmp_size <> 8)
    err 'internal error'
  end if
  db flags shl 6 + 0x02, offset
end macro

macro ebc.jump_call_auto opcode, flags, ops&
  local jmp_size, offset, imm, rel
  if (opcode = 0x03)
    jmp_size = 32 ; no CALL8
  else
    jmp_size = 8
  end if

  ebc.jump_get_offset jmp_size, rel, offset, ops
  if (jmp_size >= 0)
    if (jmp_size = 8)
      if (opcode = 0x03)
        err 'internal error'
      end if
      ebc.jump_short (flags shr 2), ops
    else if (jmp_size = 32)
      ebc.jump_call opcode, flags, R0(ops)
    else
      ebc.jump_call (0x40 or opcode), flags, ops
    end if
  else ; register or indexed => use the 32 bit version
    ebc.jump_call opcode, flags, ops
  end if
end macro

macro ebc.cmp_immediate opcode, width, ops&
  local op1, op2, op1_reg_num, op1_indirect, op1_index
  local op1_imm, op1_nat, op1_cst, op2_width, rel
  op1_index = 0
  op1_nat = 0
  op1_cst = 0
  op1_imm = 0
  op2_width = width

  match _op1 (val) =, _op2, ops
    define op1 _op1
    ebc.parse_parenthesis op1_index, op1_nat, op1_cst, op1_imm, val
    if (op1_index = 3)
      err 'syntx error'
    end if
    define op2 _op2
  else match _op1 =, _op2, ops
    define op1 _op1
    define op2 _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect, op1
  if (op1_indirect > 1)
    err 'illegal register'
  end if
  if (op1_index = 1) & (op1_indirect <> 1)
    err 'illegal direct register indexed operation'
  end if
  if (op1_nat = 0) & (op1_cst = 0)
    op1_index = 0
  end if
  if (op2_width = -1)
    is_relative rel, op2
    if (rel) | (op2 and 0xFFFF0000)
      op2_width = 1
    else
      op2_width = 0
    end if
  end if

  db op2_width shl 7 + opcode
  db op1_index shl 4 + op1_indirect shl 3 + op1_reg_num
  if (op1_index = 1)
    ebc.insert_index 16, op1_nat, op1_cst
  end if
  di op2_width+1, op2
end macro

macro ebc.load_store_sp opcode, ops&
  local op1_reg_num, op2_reg_num, op1_indirect_or_vm, op2_indirect_or_vm
  match [_op1] =, _op2, ops
    if (opcode <> 0x29)
      err 'syntax error'
    end if
    define op1 _op1
    define op2 _op2
  else match _op1 =, [_op2], ops
    if (opcode <> 0x2A)
      err 'syntax error'
    end if
    define op1 _op1
    define op2 _op2
  else
    err 'syntax error'
  end match

  ebc.parse_register op1_reg_num, op1_indirect_or_vm, op1
  ebc.parse_register op2_reg_num, op2_indirect_or_vm, op2
  if (opcode = 0x29)
    if (op1_indirect_or_vm <> 2) | (op1_reg_num <> 0)
      err 'illegal register'
    end if
    if (op2_indirect_or_vm <> 0)
      err 'syntax error'
    end if
  else if (opcode = 0x2A)
    if (op2_indirect_or_vm <> 2)
      err 'illegal register'
    end if
    if (op1_indirect_or_vm <> 0)
      err 'syntax error'
    end if
  end if

  db opcode
  db op2_reg_num shl 4 + op1_reg_num
end macro

;
; Instruction list
;

macro ADD? ops&
  ebc.math_general 0x4C, ops
end macro

macro ADD32? ops&
  ebc.math_general 0x0C, ops
end macro

macro ADD64? ops&
  ebc.math_general 0x4C, ops
end macro

macro AND? ops&
  ebc.math_general 0x54, ops
end macro

macro AND32? ops&
  ebc.math_general 0x14, ops
end macro

macro AND64? ops&
  ebc.math_general 0x54, ops
end macro

macro ASHR? ops&
  ebc.math_general 0x59, ops
end macro

macro ASHR32? ops&
  ebc.math_general 0x19, ops
end macro

macro ASHR64? ops&
  ebc.math_general 0x59, ops
end macro

macro BREAK? code
  match , code
    break ; fasmg directive
  else
    db 0x00, code
  end match
end macro

macro CALL? ops&
  ebc.jump_call_auto 0x03, 0000b, ops
end macro

macro CALL32? ops&
  ebc.jump_call 0x03, 0000b, ops
end macro

macro CALL64? ops&
  ebc.jump_call 0x43, 0000b, ops
end macro

macro CALLEX? ops&
  ebc.jump_call_auto 0x03, 0010b, ops
end macro

macro CALLEX32? ops&
  ebc.jump_call 0x03, 0010b, ops
end macro

macro CALL32EX? ops&
  ebc.jump_call 0x03, 0010b, ops
end macro

macro CALLEX64? ops&
  ebc.jump_call 0x43, 0010b, ops
end macro

macro CALL64EX? ops&
  ebc.jump_call 0x43, 0010b, ops
end macro

macro CMPeq? ops&
  ebc.math_general 0x45, ops
end macro

macro CMP32eq? ops&
  ebc.math_general 0x05, ops
end macro

macro CMP64eq? ops&
  ebc.math_general 0x45, ops
end macro

macro CMPlte? ops&
  ebc.math_general 0x46, ops
end macro

macro CMP32lte? ops&
  ebc.math_general 0x06, ops
end macro

macro CMP64lte? ops&
  ebc.math_general 0x46, ops
end macro

macro CMPgte? ops&
  ebc.math_general 0x47, ops
end macro

macro CMP32gte? ops&
  ebc.math_general 0x07, ops
end macro

macro CMP64gte? ops&
  ebc.math_general 0x47, ops
end macro

macro CMPulte? ops&
  ebc.math_general 0x48, ops
end macro

macro CMP32ulte? ops&
  ebc.math_general 0x08, ops
end macro

macro CMP64ulte? ops&
  ebc.math_general 0x48, ops
end macro

macro CMPugte? ops&
  ebc.math_general 0x49, ops
end macro

macro CMP32ugte? ops&
  ebc.math_general 0x09, ops
end macro

macro CMP64ugte? ops&
  ebc.math_general 0x49, ops
end macro

macro CMPIeq? ops&
  ebc.cmp_immediate 0x6D, -1, ops
end macro

macro CMPIweq? ops&
  ebc.cmp_immediate 0x6D, 0, ops
end macro

macro CMPIdeq? ops&
  ebc.cmp_immediate 0x6D, 1, ops
end macro

macro CMPI32eq? ops&
  ebc.cmp_immediate 0x2D, -1, ops
end macro

macro CMPI32weq? ops&
  ebc.cmp_immediate 0x2D, 0, ops
end macro

macro CMPI32deq? ops&
  ebc.cmp_immediate 0x2D, 1, ops
end macro

macro CMPI64eq? ops&
  ebc.cmp_immediate 0x6D, -1, ops
end macro

macro CMPI64weq? ops&
  ebc.cmp_immediate 0x6D, 0, ops
end macro

macro CMPI64deq? ops&
  ebc.cmp_immediate 0x6D, 1, ops
end macro

macro CMPIlte? ops&
  ebc.cmp_immediate 0x6E, -1, ops
end macro

macro CMPIwlte? ops&
  ebc.cmp_immediate 0x6E, 0, ops
end macro

macro CMPIdlte? ops&
  ebc.cmp_immediate 0x6E, 1, ops
end macro

macro CMPI32lte? ops&
  ebc.cmp_immediate 0x2E, -1, ops
end macro

macro CMPI32wlte? ops&
  ebc.cmp_immediate 0x2E, 0, ops
end macro

macro CMPI32dlte? ops&
  ebc.cmp_immediate 0x2E, 1, ops
end macro

macro CMPI64lte? ops&
  ebc.cmp_immediate 0x6E, -1, ops
end macro

macro CMPI64wlte? ops&
  ebc.cmp_immediate 0x6E, 0, ops
end macro

macro CMPI64dlte? ops&
  ebc.cmp_immediate 0x6E, 1, ops
end macro

macro CMPIgte? ops&
  ebc.cmp_immediate 0x6F, -1, ops
end macro

macro CMPIwgte? ops&
  ebc.cmp_immediate 0x6F, 0, ops
end macro

macro CMPIdgte? ops&
  ebc.cmp_immediate 0x6F, 1, ops
end macro

macro CMPI32gte? ops&
  ebc.cmp_immediate 0x2F, -1, ops
end macro

macro CMPI32wgte? ops&
  ebc.cmp_immediate 0x2F, 0, ops
end macro

macro CMPI32dgte? ops&
  ebc.cmp_immediate 0x2F, 1, ops
end macro

macro CMPI64gte? ops&
  ebc.cmp_immediate 0x6F, -1, ops
end macro

macro CMPI64wgte? ops&
  ebc.cmp_immediate 0x6F, 0, ops
end macro

macro CMPI64dgte? ops&
  ebc.cmp_immediate 0x6F, 1, ops
end macro

macro CMPIulte? ops&
  ebc.cmp_immediate 0x70, -1, ops
end macro

macro CMPIwulte? ops&
  ebc.cmp_immediate 0x70, 0, ops
end macro

macro CMPIdulte? ops&
  ebc.cmp_immediate 0x70, 1, ops
end macro

macro CMPI32ulte? ops&
  ebc.cmp_immediate 0x30, -1, ops
end macro

macro CMPI32wulte? ops&
  ebc.cmp_immediate 0x30, 0, ops
end macro

macro CMPI32dulte? ops&
  ebc.cmp_immediate 0x30, 1, ops
end macro

macro CMPI64ulte? ops&
  ebc.cmp_immediate 0x70, -1, ops
end macro

macro CMPI64wulte? ops&
  ebc.cmp_immediate 0x70, 0, ops
end macro

macro CMPI64dulte? ops&
  ebc.cmp_immediate 0x70, 1, ops
end macro

macro CMPIugte? ops&
  ebc.cmp_immediate 0x71, -1, ops
end macro

macro CMPIwugte? ops&
  ebc.cmp_immediate 0x71, 0, ops
end macro

macro CMPIdugte? ops&
  ebc.cmp_immediate 0x71, 1, ops
end macro

macro CMPI32ugte? ops&
  ebc.cmp_immediate 0x31, -1, ops
end macro

macro CMPI32wugte? ops&
  ebc.cmp_immediate 0x31, 0, ops
end macro

macro CMPI32dugte? ops&
  ebc.cmp_immediate 0x31, 1, ops
end macro

macro CMPI64ugte? ops&
  ebc.cmp_immediate 0x71, -1, ops
end macro

macro CMPI64wugte? ops&
  ebc.cmp_immediate 0x71, 0, ops
end macro

macro CMPI64dugte? ops&
  ebc.cmp_immediate 0x71, 1, ops
end macro

macro DIV? ops&
  ebc.math_general 0x50, ops
end macro

macro DIV32? ops&
  ebc.math_general 0x10, ops
end macro

macro DIV64? ops&
  ebc.math_general 0x50, ops
end macro

macro DIVU? ops&
  ebc.math_general 0x51, ops
end macro

macro DIVU32? ops&
  ebc.math_general 0x11, ops
end macro

macro DIVU64? ops&
  ebc.math_general 0x51, ops
end macro

macro EXTNDB? ops&
  ebc.math_general 0x5A, ops
end macro

macro EXTNDB32? ops&
  ebc.math_general 0x1A, ops
end macro

macro EXTNDB64? ops&
  ebc.math_general 0x5A, ops
end macro

macro EXTNDD? ops&
  ebc.math_general 0x5C, ops
end macro

macro EXTNDD32? ops&
  ebc.math_general 0x1C, ops
end macro

macro EXTNDD64? ops&
  ebc.math_general 0x5C, ops
end macro

macro EXTNDW? ops&
  ebc.math_general 0x5B, ops
end macro

macro EXTNDW32? ops&
  ebc.math_general 0x1B, ops
end macro

macro EXTNDW64? ops&
  ebc.math_general 0x5B, ops
end macro

macro JMP? ops&
  ebc.jump_call_auto 0x01, 0001b, ops
end macro

macro JMPcs? ops&
  ebc.jump_call_auto 0x01, 1101b, ops
end macro

macro JMPcc? ops&
  ebc.jump_call_auto 0x01, 1001b, ops
end macro

macro JMP32? ops&
  ebc.jump_call 0x01, 0000b, ops
end macro

macro JMP32cs? ops&
  ebc.jump_call 0x01, 1100b, ops
end macro

macro JMP32cc? ops&
  ebc.jump_call 0x01, 1000b, ops
end macro

macro JMP64? ops&
  ebc.jump_call 0x41, 0000b, ops
end macro

macro JMP64cs? ops&
  ebc.jump_call 0x41, 1100b, ops
end macro

macro JMP64cc? ops&
  ebc.jump_call 0x41, 1000b, ops
end macro

macro JMP8? ops&
  ebc.jump_short 00b, ops
end macro

macro JMP8cc? ops&
  ebc.jump_short 10b, ops
end macro

macro JMP8cs? ops&
  ebc.jump_short 11b, ops
end macro

macro LOADSP? ops&
  ebc.load_store_sp 0x29, ops
end macro

macro MOD? ops&
  ebc.math_general 0x52, ops
end macro

macro MOD32? ops&
  ebc.math_general 0x12, ops
end macro

macro MOD64? ops&
  ebc.math_general 0x52, ops
end macro

macro MODU? ops&
  ebc.math_general 0x53, ops
end macro

macro MODU32? ops&
  ebc.math_general 0x13, ops
end macro

macro MODU64? ops&
  ebc.math_general 0x53, ops
end macro

macro MOV? ops&
  ebc.move_general 0x120, ops
end macro

macro MOVbw? ops&
  ebc.move_general 0x1D, ops
end macro

macro MOVb? ops&
  ebc.move_general 0x11D, ops
end macro

macro MOVww? ops&
  ebc.move_general 0x1E, ops
end macro

macro MOVw? ops&
  ebc.move_general 0x11E, ops
end macro

macro MOVdw? ops&
  ebc.move_general 0x1F, ops
end macro

macro MOVd? ops&
  ebc.move_general 0x11F, ops
end macro

macro MOVqw? ops&
  ebc.move_general 0x20, ops
end macro

macro MOVq? ops&
  ebc.move_general 0x120, ops
end macro

macro MOVbd? ops&
  ebc.move_general 0x21, ops
end macro

macro MOVwd? ops&
  ebc.move_general 0x22, ops
end macro

macro MOVdd? ops&
  ebc.move_general 0x23, ops
end macro

macro MOVqd? ops&
  ebc.move_general 0x23, ops
end macro

macro MOVqq? ops&
  ebc.move_general 0x28, ops
end macro

macro MOVI? ops&
  ebc.move_imm_ind_rel 3, -1, ops
end macro

macro MOVIb? ops&
  ebc.move_imm_ind_rel 0, -1, ops
end macro

macro MOVIbw? ops&
  ebc.move_imm_ind_rel 0, 1, ops
end macro

macro MOVIbd? ops&
  ebc.move_imm_ind_rel 0, 2, ops
end macro

macro MOVIbq? ops&
  ebc.move_imm_ind_rel 0, 3, ops
end macro

macro MOVIw? ops&
  ebc.move_imm_ind_rel 1, -1, ops
end macro

macro MOVIww? ops&
  ebc.move_imm_ind_rel 1, 1, ops
end macro

macro MOVIwd? ops&
  ebc.move_imm_ind_rel 1, 2, ops
end macro

macro MOVIwq? ops&
  ebc.move_imm_ind_rel 1, 3, ops
end macro

macro MOVId? ops&
  ebc.move_imm_ind_rel 2, -1, ops
end macro

macro MOVIdw? ops&
  ebc.move_imm_ind_rel 2, 1, ops
end macro

macro MOVIdd? ops&
  ebc.move_imm_ind_rel 2, 2, ops
end macro

macro MOVIdq? ops&
  ebc.move_imm_ind_rel 2, 3, ops
end macro

macro MOVIq? ops&
  ebc.move_imm_ind_rel 3, -1, ops
end macro

macro MOVIqw? ops&
  ebc.move_imm_ind_rel 3, 1, ops
end macro

macro MOVIqd? ops&
  ebc.move_imm_ind_rel 3, 2, ops
end macro

macro MOVIqq? ops&
  ebc.move_imm_ind_rel 3, 3, ops
end macro

macro MOVIn? ops&
  ebc.move_imm_ind_rel -1, -1, ops
end macro

macro MOVInw? ops&
  ebc.move_imm_ind_rel -1, 1, ops
end macro

macro MOVInd? ops&
  ebc.move_imm_ind_rel -1, 2, ops
end macro

macro MOVInq? ops&
  ebc.move_imm_ind_rel -1, 3, ops
end macro

macro MOVn? ops&
  ebc.move_general 0x132, ops
end macro

macro MOVnw? ops&
  ebc.move_general 0x32, ops
end macro

macro MOVnd? ops&
  ebc.move_general 0x33, ops
end macro

macro MOVREL? ops&
  ebc.move_imm_ind_rel -2, -1, ops
end macro

macro MOVRELw? ops&
  ebc.move_imm_ind_rel -2, 1, ops
end macro

macro MOVRELd? ops&
  ebc.move_imm_ind_rel -2, 2, ops
end macro

macro MOVRELq? ops&
  ebc.move_imm_ind_rel -2, 3, ops
end macro

macro MOVsn? ops&
  ebc.move_general 0x125, ops
end macro

macro MOVsnw? ops&
  ebc.move_general 0x25, ops
end macro

macro MOVsnd? ops&
  ebc.move_general 0x26, ops
end macro

macro MUL? ops&
  ebc.math_general 0x4E, ops
end macro

macro MUL32? ops&
  ebc.math_general 0x0E, ops
end macro

macro MUL64? ops&
  ebc.math_general 0x4E, ops
end macro

macro MULU? ops&
  ebc.math_general 0x4F, ops
end macro

macro MULU32? ops&
  ebc.math_general 0x0F, ops
end macro

macro MULU64? ops&
  ebc.math_general 0x4F, ops
end macro

macro NEG? ops&
  ebc.math_general 0x4B, ops
end macro

macro NEG32? ops&
  ebc.math_general 0x0B, ops
end macro

macro NEG64? ops&
  ebc.math_general 0x4B, ops
end macro

macro NOT? ops&
  ebc.math_general 0x4A, ops
end macro

macro NOT32? ops&
  ebc.math_general 0x0A, ops
end macro

macro NOT64? ops&
  ebc.math_general 0x4A, ops
end macro

macro OR? ops&
  ebc.math_general 0x55, ops
end macro

macro OR32? ops&
  ebc.math_general 0x15, ops
end macro

macro OR64? ops&
  ebc.math_general 0x55, ops
end macro

macro POP? ops&
  ebc.push_pop 0x6C, ops
end macro

macro POP32? ops&
  ebc.push_pop 0x2C, ops
end macro

macro POP64? ops&
  ebc.push_pop 0x6C, ops
end macro

macro POPn? ops&
  ebc.push_pop 0x36, ops
end macro

macro PUSH? ops&
  ebc.push_pop 0x6B, ops
end macro

macro PUSH32? ops&
  ebc.push_pop 0x2B, ops
end macro

macro PUSH64? ops&
  ebc.push_pop 0x6B, ops
end macro

macro PUSHn? ops&
  ebc.push_pop 0x35, ops
end macro

macro RET?
  dw 0x0004
end macro

macro SHL? ops&
  ebc.math_general 0x57, ops
end macro

macro SHL32? ops&
  ebc.math_general 0x17, ops
end macro

macro SHL64? ops&
  ebc.math_general 0x57, ops
end macro

macro SHR? ops&
  ebc.math_general 0x58, ops
end macro

macro SHR32? ops&
  ebc.math_general 0x18, ops
end macro

macro SHR64? ops&
  ebc.math_general 0x58, ops
end macro

macro STORESP? ops&
  ebc.load_store_sp 0x2A, ops
end macro

macro SUB? ops&
  ebc.math_general 0x4D, ops
end macro

macro SUB32? ops&
  ebc.math_general 0x0D, ops
end macro

macro SUB64? ops&
  ebc.math_general 0x4D, ops
end macro

macro XOR? ops&
  ebc.math_general 0x56, ops
end macro

macro XOR32? ops&
  ebc.math_general 0x16, ops
end macro

macro XOR64? ops&
  ebc.math_general 0x56, ops
end macro

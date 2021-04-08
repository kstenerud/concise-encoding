Concise Encoding Schema
=======================

A schema specification for Concise Encoding documents.


**Note:** This is an early work-in-progress.



Design Notes
------------

These are my personal notes made during various design cycles. Don't expect them to make much sense.


Schema:
- id
- parent
- include


Type:
- string
- integer
- float
- number

String:
- length
- min max length
- regex match
- preset formats (hostnames, ip address, etc)

Number:
- multiple of
- min, max (inclusive, exclusive)

Scalar:
- allowed values

Map:
- ordered/unordered
- allowed keys
- required keys
- disallowed keys
- allow unspecified values
- min/max number of values
- dependency upon another key
- key name regex

List:
- ordered/unordered
- unique
- element constraints
- min/max length
- per-index constraints (tuples)

Default values
Examples

All of
Any of
One of
Not

If: true path, false path

-------------------------------------------------

Types:
- Nil        
- Boolean    
- Integer    
- Float      
- UUID       
- Time       
- String     
- URI        
- Array(type)
- List       
- Map        
- Markup     
- Reference  
- Comment    
- Custom     

Allowed values:
- list of values
- list of patterns
- min/max value
- min/max length

Disallowed values:
- list of values
- list of patterns

------------------------------------

Value Constraint:
- List of type constraints

Type constraint:
- type
- constraint

constraint:
- list of values
- list of patterns
- list of ranges (numeric types) (inclusive, exclusive)
- list of length ranges (array types)

Constraint containers:
- one of
- all of
- any of
- not

Map:
- ordered/unordered
- max element count
- map of key: {constraint, required}
- general key constraint for keys not in above map
- general value constraint for values not from keys in above map

dependencies...
XML schema:

data types: string, decimal, integer, boolean, date, time

<xs:element name = "Address">
   <xs:complexType>
      <xs:sequence>
         <xs:element name = "name" type = "xs:string" />
         <xs:element name = "company" type = "xs:string" />
         <xs:element name = "phone" type = "xs:int" /> 
      </xs:sequence> 
   </xs:complexType>
</xs:element> 

Element Attributes:
<xs:attribute name="lang" type="xs:string"/> 

Restrictions:

<xs:element name="age">
  <xs:simpleType>
    <xs:restriction base="xs:integer">
      <xs:minInclusive value="0"/>
      <xs:maxInclusive value="120"/>
    </xs:restriction>
  </xs:simpleType>
</xs:element> 

enumeration 	Defines a list of acceptable values
fractionDigits 	Specifies the maximum number of decimal places allowed. Must be equal to or greater than zero
length 			Specifies the exact number of characters or list items allowed. Must be equal to or greater than zero
maxExclusive 	Specifies the upper bounds for numeric values (the value must be less than this value)
maxInclusive 	Specifies the upper bounds for numeric values (the value must be less than or equal to this value)
maxLength 		Specifies the maximum number of characters or list items allowed. Must be equal to or greater than zero
minExclusive 	Specifies the lower bounds for numeric values (the value must be greater than this value)
minInclusive 	Specifies the lower bounds for numeric values (the value must be greater than or equal to this value)
minLength 		Specifies the minimum number of characters or list items allowed. Must be equal to or greater than zero
pattern 		Defines the exact sequence of characters that are acceptable
totalDigits 	Specifies the exact number of digits allowed. Must be greater than zero
whiteSpace 		Specifies how white space (line feeds, tabs, spaces, and carriage returns) is handled

Custom types:
<xs:element name="employee" type="personinfo"/>
<xs:element name="student" type="personinfo"/>
<xs:element name="member" type="personinfo"/>

<xs:complexType name="personinfo">
  <xs:sequence>
    <xs:element name="firstname" type="xs:string"/>
    <xs:element name="lastname" type="xs:string"/>
  </xs:sequence>
</xs:complexType> 

--------------------------------------

- default values
- fixed values (cannot be changed)
- whitespace preserve/replace. Needed? https://www.w3schools.com/xml/schema_facets.asp
- ordering

General constraints:
- true, false
- one, all, any, not, must, maybe
- if (true path, false path)

Comparables:
- lt, gt, lte, gte, eq

Container-like:
- length (lt,gt,lte,gte,eq)

String-like:
- regex

Numeric:
- multiple of

Nil:

Boolean:

Integer:

Float:
- max sig digits?
- max fractional digits

UUID:

Time:
- (time unit)s of (time unit)
- probably lots more

String:

URI:
- values in parts of URI?

Array(type):
- numeric constraints based on type

List:
- ordered/unordered
- unique
- element type constraints
- per-index constraints

Map:
- ordered/unordered
- per-element constraint
- default element constraint

Map element:
- key constraint
- value constraint
- dependency on another element

Markup:

Not allowed:
- Reference
- Comment
- Custom

-----------------------------------
```
constraints = [
    &constr1:{
    	oneof = {
    		bool = {
    			eq = false
	    	}
    		int = {
    			ge = 1
    			le = 10
	    	}
	    }
    }
]
```
```
constraints = [
    &constr1:[
    	{
    		t=oneof
    		list=[
    			{
    				t=bool
    				eq=false
    			}
    			{
    				t=int
	    			ge = 1
	    			le = 10
    			}
	    	}
	    }
    }
]
```
```
<constraints|
	&constr1:<oneof|
		<bool eq=false>
		<int ge=1 le=10|
			<not|
				<oneof|
					<value _=5>
					<value _=7>
				>
			>
		>
	>
>
```
```
<constraints|
	<oneof id=constr1|
		<bool eq=false>
		<int ge=1 le=10|
			<not|<oneof|
				<value _=5>
				<value _=7>
			>>
		>
	>
>
```

Contents are AND by default. Needed? allof does this.

general attributes:
- id=someid
- depends=someid
- eq,ne,gt,ge,lt,le=something

Type-specific attributes:
- pattern=
- optional=true: If map key type constraint passes, all must pass

- <sometype ...>

- <pattern _=xxx>
- <true> always succeeds
- <false> always fails
- <eq _=something>
- <ne _=something>
- <gt _=something>
- <ge _=something>
- <lt _=something>
- <le _=something>
- <not| items...>
- <oneof| items...>
- <allof| items...>
- <anyof| items...>
- <if ...> Not sure if this is a good idea.


Optional: Some kind of "if this is present, validate it". Make it an attribute?
- optional only matters in "allof"
- optional only works on the key field of a map, in "allof"
- optional only triggers the internal validation if the top-level validation succeeds.
- is optional just "anyof"?

<allof|
  <string _="a">
  <string _="b">
  <anyof|
  	<int _=10>
  >
>

<allof|
  <string _="a">
  <string _="b">
  <int _=10 optional=true>
>

doesn't work. How to get list of map key validators?


<map|
  <field optional=true type=string _=width|
  	<string _=width>
  	<int ge=0 le=100>
  >
  <field string _="a">
  <field string _="b">
>

<field name=abc|
  constraints...
>

<field pattern=xyz>
<field optional=true>


<list|
  <index _=0>
>

---------------------------------

.csb, .cst
.cbs, .cts

- or dt=u"https://concise-encoding.org/schema/v1.0"
------------------------------------------------------------

How to use references in markup?

```
&someid:<something>
$someid

&dollar; to print actual $
Option: \$ to directly escape $... maybe not.
```

---------------------------------------------------

<int
	eq=1
	ne=500
	lt=10
	le=10
	gt=0
	ge=0
	|

>

-----------------------------
map:
- collection of field:
	- key (keyable type)
	- value (any type)
	- default
	- optional
	- fixed value?
- ordered/unordered
- general key constraint
- general value constraint
- length constraint
- allow unspecified keys

list:
- collection of indices:
	- ordinal (pos int)
	- value (any type)
	- default?
	- optional?
- unique
- general value constraint
- length constraint
- contains (constraint) - or maybe atleast(count, constraint), atmost(count, constraint)

array:
- collection of indices:
	- ordinal (pos int)
	- value (array element type)
	- default?
	- optional?
- unique
- general value constraint
- length constraint

markup:
- has attributes map
	- same as map
- has contents
	- element
		- occurrances constraint
	- sequence of elements
	- oneof elements
	- anyof elements
	- rules for text content?
	- any, any-attribute

Value:
- compare to xyz (eq,ne,lt,gt,le,ge)
- length compare to xyz
- pattern match
- multiple of

String:
- length constraint
- pattern

Float:
- max sig digits?
- max fractional digits

Time:
- (time unit)s of (time unit)

URI:
- parts of URI constraints

General:
- not
- if test constraint, then constraint, else constraint

Attributes:
- id
- depends

Default values?

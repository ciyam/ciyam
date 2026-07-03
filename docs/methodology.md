Introduction to Software Manufacturing
--------------------------------------

The goal of the CIYAM software engineering methodology (known as Software Manufacturing) is to give developers
a way of quickly producing quality applications through the re-use of tried and tested software components and
source code.

The goal in itself is not really much different to what you would expect from nearly any software platform but
what is most different about CIYAM's Software Manufacturing approach is that applications that are constructed
with it will not contain any manually written source code.

This perhaps requires repeating to underline its importance, there simply is *no* manually written source code
in a completed software application that is constructed using CIYAM's Software Manufacturing methodology.

No manual source code - really?
-------------------------------

This is a point that many find rather hard to swallow and it needs to be carefully explained so it is clear as
to exactly how this works (as there is nothing "magical" going on here).

Beyond the standard application modelling artifacts (such as classes, fields, procedures, etc.) there are more
detailed artifacts known as "specifications" that provide a means of using source code snippet templates which
(for the most part) will be "tried and tested" (read on) that operate with the other model artifacts.

The obvious question now arises - "What if no existing specification (or set of specifications) can be used to
generate the source code desired?". This is a good question and a core part of the difference between this and
other methodologies. The answer - if required you protoype code manually and after thoroughly testing it works
you transform this code into one or more new source code templates and create one or more new "specifications"
which will provide the information necessary to generate the source code from the new templates.

Thus manually written source code has become new source code templates and specification types. This is not so
hard to follow but once understood tends to lead to the next question "Doesn't that require more work to write
the code?" and the short answer to this is "Yes - indeed it does!".

Why insist upon this extra work?
--------------------------------

At first it can seem as though this approach is just burdening software engineers with yet more work (and this
will be especially viewed this way if the code is not seen as being likely to ever be re-used in the future).

Here then are the major reasons (and reasoning) as to why this approach is insisted upon:

1) It discourages manually written code.

If the software engineer knows that all manually written code will need to be turned into source templates and
specification types before being accepted then they are far more likely to instead work with the existing (and
thus more "tried and tested") source templates and specification types.

The encouragement for re-use that the above discouragement implies has the side benefit that if there are bugs
or deficiencies in existing source templates and/or specifications then these will be more quickly located and
repaired/improved (with all existing software gaining these changes immediately upon their next regeneration).

2) It encourages mimimal amounts of new manually written code.

This is perhaps just a further extension of the first point but is separated out to make it clear that for the
software engineer writing new source code templates and specification types that work well with other existing
ones will greatly reduce their workload and therefore will likely become a natural preference to writing large
amounts of source code that will then have to be converted into complicated source templates and specification
types.

3) The benefit of artifact renaming support as well as future re-use.

The creation of small source code templates and specification types that work well with others can help ensure
that even code thought to be quite "one off" may find a later re-use (at which point that extra work will have
paid off).

The other often overlooked advantage to having all the source code being generated in the manner that is being
done by CIYAM's Software Manufacturing methodology is that the renaming of model artifacts (such as classes or
fields, etc.) will require only software regeneration (so no time will be wasted doing search and replaces).

How is this better to traditional approaches?
---------------------------------------------

The encouragement of re-use of well tried and tested software source templates over the manual creation of new
source code is fairly obvious but here are a few other reasons.

1) The "copy and paste" approach to writing software is replaced with a "select and generate" one.

It is not hard for any software engineer who has worked on non-trivial software applications to understand the
evils of "copy and paste" (as generally this activity actually becomes "copy, paste and replace" as the source
code at the destination is rarely going to be identical to that of the origin). An even worse problem than the
"oops - forgot to change that" when using this approach is the fact that if one or more bugs are hiding in the
original source then you have the sometimes quite difficult task of trying to find every place where this code
was copied to in order to make the same changes needed to fix them.

Of course to this point an immediate response is "but that is what using objects/procedures/templates/etc." is
supposed to provide (in terms of having a single implementation) and to a large extent such approaches can and
do address this, however, there are limitations to all such language constructs which is why in so many places
one sees small isolated "code fragments" (or snippets) appearing via the clipboard (as well as just the common
"lazy programmer").

So programmers faced with a "no copy and paste here" rule will either quit in frustration (maybe a good thing)
or instead try to work out which specification types might provide source code to perform the function that is
required (the "select and generate" approach).

2) A new and very clear division of duties suitable for different kinds of programmers.

As programmers become more familiar with the "select and generate" approach to rapidly creating software it is
quite possible that they end up with less interest in wanting to create new source templates and specification
types whilst other programmers may find more interest in doing just that (with the experience and skill levels
of individuals being an equally important factor in deciding who will do what).

Traditional software development approaches are usually clear about these divisions of duties when it comes to
creating new or to changing common (or library) code but as pointed out above when manual coding is allowed to
be performed it is not so clear as to when a snippet should become part of the common code (which will require
more time to be allocated to code reviews).

By making it not possible to "sneak in" some code fragment/snippet the software construction process itself is
able to assist in making sure only well tested such things are included via specifications (which will require
less time to be expended on code reviews).


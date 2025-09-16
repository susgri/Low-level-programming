
def isNotException(letter):
     exceptions = ['?', '#']
     return letter not in exceptions 

def isSpace(letter):
     return letter == ' '

def findPalindrome(input):
    isPalindrome = 1
    length = len(input)
    word = input.lower()
    i = 0
    j = (length-1)-i

    while i < length//2 and j > length//2 :
        letter1 = word[i]
        letter2 = word[j]

        # check for space
        if isSpace(letter1) and not isSpace(letter2):
            i += 1
            letter1 = word[i]
        elif isSpace(letter2) and not isSpace(letter1):
            j -= 1
            letter2 = word[j]

        if letter1 != letter2:
            if isNotException(letter1) and isNotException(letter2):
                isPalindrome = 0

        i += 1
        j -= 1

    return isPalindrome


print(f"Is palindrome: {findPalindrome("First level")}")
print(f"Is palindrome: {findPalindrome("My gym")}")
print(f"Is palindrome: {findPalindrome('Was it a car or a cat I saw')}")